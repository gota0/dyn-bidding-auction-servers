//  Copyright 2024 Google LLC
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "services/common/util/hash_util.h"

#include <gmock/gmock-matchers.h>

#include <optional>
#include <string>

#include "absl/strings/escaping.h"
#include "absl/strings/string_view.h"
#include "gtest/gtest.h"

namespace privacy_sandbox::bidding_auction_servers {
namespace {

HashUtil hash_util_ = HashUtil();

// Copied from Chromium
// (third_party/blink/common/interest_group/interest_group_unittest.cc).
template <int N>
std::string MakeNullSafeString(const char (&as_chars)[N]) {
  return std::string(as_chars, N - 1);
}

MATCHER_P(ByteStringEq, value, "") {
  if (value.size() != arg.size()) {
    return false;
  }

  for (int i = 0; i < value.size(); ++i) {
    if (value[i] != arg[i]) {
      return false;
    }
  }

  return true;
}

// Copied from Chromium (crypto/sha2_unittest.cc).
// These hashes were ensured to be equivalent to the hashes generated by
// Chromium.
TEST(HashUtilTest, ComputeSHA256) {
  EXPECT_EQ(ComputeSHA256(""),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  EXPECT_EQ(ComputeSHA256("abc"),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
  EXPECT_EQ(
      ComputeSHA256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
      "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
  EXPECT_EQ(
      ComputeSHA256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
      "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
  std::string million_As(1000000, 'a');
  EXPECT_EQ(ComputeSHA256(million_As),
            "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
  EXPECT_THAT(
      ComputeSHA256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
                    /*return_hex=*/false),
      ByteStringEq(absl::HexStringToBytes(
          "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1")));
}

TEST(HashUtilTest, CanonicalizeURL) {
  EXPECT_EQ(CanonicalizeURL("HTTPS://www.example.com"),
            "https://www.example.com/");
  EXPECT_EQ(CanonicalizeURL("HTTPS://www.example.com/"),
            "https://www.example.com/");
  EXPECT_EQ(CanonicalizeURL("HTTPS://www.example.com/../path"),
            "https://www.example.com/path");
  EXPECT_EQ(CanonicalizeURL("HTTPS://www.example.com/./path"),
            "https://www.example.com/path");
  EXPECT_EQ(CanonicalizeURL("HTTPS://www.example.com/path/./"),
            "https://www.example.com/path/");
  EXPECT_EQ(CanonicalizeURL("HTTPS://www.example.com/path/./././"),
            "https://www.example.com/path/");
}

TEST(HashUtilTest, PlainTextKAnonKeyForAdRenderURL) {
  EXPECT_EQ(
      PlainTextKAnonKeyForAdRenderURL(
          /*owner=*/"https://example.org",
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad1.com"),
      "AdBid\n"
      "https://example.org/\nhttps://example.org/bid.js\nhttps://ad1.com/");
  EXPECT_EQ(
      PlainTextKAnonKeyForAdRenderURL(
          /*owner=*/"https://example.com",
          /*bidding_url=*/"https://example.org/bidding.js",
          /*render_url=*/"https://ad2.com"),
      "AdBid\n"
      "https://example.com/\nhttps://example.org/bidding.js\nhttps://ad2.com/");
}

TEST(HashUtilTest, PlainTextKAnonKeyForAdComponentRenderURL) {
  EXPECT_EQ(PlainTextKAnonKeyForAdComponentRenderURL(
                /*component_render_url=*/"https://ad1.com"),
            "ComponentBid\nhttps://ad1.com/");
  EXPECT_EQ(PlainTextKAnonKeyForAdComponentRenderURL(
                /*component_render_url=*/"https://ad2.com"),
            "ComponentBid\nhttps://ad2.com/");
}

// Copied from Chromium
// (third_party/blink/common/interest_group/interest_group_unittest.cc).
// These plain text hash keys were ensured to be equivalent to the hashes
// generated by Chromium.
TEST(HashUtilTest, PlainTextKAnonKeyForReportingID) {
  // Make sure that PlainTextKAnonKeyForReportingID properly prioritizes
  // and incorporates various kinds of reporting IDs.
  KAnonKeyReportingIDParam id_param_1 = {};
  KAnonKeyReportingIDParam id_param_2 = {.buyer_reporting_id = "bid"};
  KAnonKeyReportingIDParam id_param_3 = {.buyer_and_seller_reporting_id =
                                             "bsid"};
  KAnonKeyReportingIDParam id_param_4 = {
      .buyer_reporting_id = "bid", .buyer_and_seller_reporting_id = "bsid"};
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad1.com", id_param_1),
      "NameReport\n"
      "https://example.org/\nhttps://example.org/bid.js\nhttps://ad1.com/\n"
      "ig_one");
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad2.com", id_param_2),
      "BuyerReportId\n"
      "https://example.org/\nhttps://example.org/bid.js\nhttps://ad2.com/\n"
      "bid");
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad3.com", id_param_3),
      "BuyerAndSellerReportId\n"
      "https://example.org/\nhttps://example.org/bid.js\nhttps://ad3.com/\n"
      "bsid");
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad3.com", id_param_4),
      "BuyerAndSellerReportId\n"
      "https://example.org/\nhttps://example.org/bid.js\nhttps://ad3.com/\n"
      "bsid");

  KAnonKeyReportingIDParam id_param_5 = {
      .selected_buyer_and_seller_reporting_id = "sbsid"};
  KAnonKeyReportingIDParam id_param_6 = {
      .buyer_reporting_id = "bid",
      .selected_buyer_and_seller_reporting_id = "sbsid"};
  KAnonKeyReportingIDParam id_param_7 = {
      .buyer_and_seller_reporting_id = "bsid",
      .selected_buyer_and_seller_reporting_id = "sbsid"};
  KAnonKeyReportingIDParam id_param_8 = {
      .buyer_reporting_id = "bid",
      .buyer_and_seller_reporting_id = "bsid",
      .selected_buyer_and_seller_reporting_id = "sbsid"};
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad1.com", id_param_5),
      MakeNullSafeString(
          "SelectedBuyerAndSellerReportId\n"
          "https://example.org/\nhttps://example.org/bid.js\nhttps://ad1.com/\n"
          "\x01\x00\x00\x00\x05"
          "sbsid\n"
          "\x00\x00\x00\x00\x00\n"
          "\x00\x00\x00\x00\x00"));
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad2.com", id_param_6),
      MakeNullSafeString(
          "SelectedBuyerAndSellerReportId\n"
          "https://example.org/\nhttps://example.org/bid.js\nhttps://ad2.com/\n"
          "\x01\x00\x00\x00\x05"
          "sbsid\n"
          "\x00\x00\x00\x00\x00\n"
          "\x01\x00\x00\x00\x03"
          "bid"));
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad3.com", id_param_7),
      MakeNullSafeString(
          "SelectedBuyerAndSellerReportId\n"
          "https://example.org/\nhttps://example.org/bid.js\nhttps://ad3.com/\n"
          "\x01\x00\x00\x00\x05"
          "sbsid\n"
          "\x01\x00\x00\x00\x04"
          "bsid\n"
          "\x00\x00\x00\x00\x00"));
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad3.com", id_param_8),
      MakeNullSafeString(
          "SelectedBuyerAndSellerReportId\n"
          "https://example.org/\nhttps://example.org/bid.js\nhttps://ad3.com/\n"
          "\x01\x00\x00\x00\x05"
          "sbsid\n"
          "\x01\x00\x00\x00\x04"
          "bsid\n"
          "\x01\x00\x00\x00\x03"
          "bid"));
}

// Copied from Chromium
// (third_party/blink/common/interest_group/interest_group_unittest.cc).
// These plain text hash keys were ensured to be equivalent to the hashes
// generated by Chromium.
TEST(InterestGroupTest, PlainTextKAnonKeyForReportingIDWithSpecialCharacters) {
  std::string buyer_id = MakeNullSafeString("b\x00\x01\nid");
  std::string buyer_seller_id = MakeNullSafeString("b\x00\x01\nsid");
  std::string selected_id = MakeNullSafeString("s\x00\x01\nbsid");

  KAnonKeyReportingIDParam id_param_1 = {};
  KAnonKeyReportingIDParam id_param_2 = {.buyer_reporting_id = buyer_id};
  KAnonKeyReportingIDParam id_param_3 = {
      .buyer_reporting_id = buyer_id,
      .buyer_and_seller_reporting_id = buyer_seller_id};

  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org",
          /*ig_name=*/MakeNullSafeString("i\x00\x01\ng_one"),
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad1.com", id_param_1),
      MakeNullSafeString(
          "NameReport\n"
          "https://example.org/\nhttps://example.org/bid.js\nhttps://ad1.com/\n"
          "i\x00\x01\ng_one"));
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org",
          /*ig_name=*/MakeNullSafeString("i\x00\x01\ng_one"),
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad2.com", id_param_2),
      MakeNullSafeString(
          "BuyerReportId\n"
          "https://example.org/\nhttps://example.org/bid.js\nhttps://ad2.com/\n"
          "b\x00\x01\nid"));
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org",
          /*ig_name=*/MakeNullSafeString("i\x00\x01\ng_one"),
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad3.com", id_param_3),
      MakeNullSafeString(
          "BuyerAndSellerReportId\n"
          "https://example.org/\nhttps://example.org/bid.js\nhttps://ad3.com/\n"
          "b\x00\x01\nsid"));

  KAnonKeyReportingIDParam id_param_4 = {
      .selected_buyer_and_seller_reporting_id = selected_id};
  KAnonKeyReportingIDParam id_param_5 = {
      .buyer_reporting_id = buyer_id,
      .selected_buyer_and_seller_reporting_id = selected_id};
  KAnonKeyReportingIDParam id_param_6 = {
      .buyer_reporting_id = buyer_id,
      .buyer_and_seller_reporting_id = buyer_seller_id,
      .selected_buyer_and_seller_reporting_id = selected_id};
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org",
          /*ig_name=*/MakeNullSafeString("i\x00\x01\ng_one"),
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad1.com", id_param_4),
      MakeNullSafeString(
          "SelectedBuyerAndSellerReportId\n"
          "https://example.org/\nhttps://example.org/bid.js\nhttps://ad1.com/\n"
          "\x01\x00\x00\x00\x08"
          "s\x00\x01\nbsid\n"
          "\x00\x00\x00\x00\x00\n"
          "\x00\x00\x00\x00\x00"));
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org",
          /*ig_name=*/MakeNullSafeString("i\x00\x01\ng_one"),
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad2.com", id_param_5),
      MakeNullSafeString(
          "SelectedBuyerAndSellerReportId\n"
          "https://example.org/\nhttps://example.org/bid.js\nhttps://ad2.com/\n"
          "\x01\x00\x00\x00\x08"
          "s\x00\x01\nbsid\n"
          "\x00\x00\x00\x00\x00\n"
          "\x01\x00\x00\x00\x06"
          "b\x00\x01\nid"));
  EXPECT_EQ(
      PlainTextKAnonKeyForReportingID(
          /*owner=*/"https://example.org",
          /*ig_name=*/MakeNullSafeString("i\x00\x01\ng_one"),
          /*bidding_url=*/"https://example.org/bid.js",
          /*render_url=*/"https://ad3.com", id_param_6),
      MakeNullSafeString(
          "SelectedBuyerAndSellerReportId\n"
          "https://example.org/\nhttps://example.org/bid.js\nhttps://ad3.com/\n"
          "\x01\x00\x00\x00\x08"
          "s\x00\x01\nbsid\n"
          "\x01\x00\x00\x00\x07"
          "b\x00\x01\nsid\n"
          "\x01\x00\x00\x00\x06"
          "b\x00\x01\nid"));
}

TEST(InterestGroupTest, HashedKAnonKeyReturnsDistinctHashes) {
  KAnonKeyReportingIDParam id_param_1 = {
      .buyer_reporting_id = "buyer_id",
      .selected_buyer_and_seller_reporting_id = "selected_id"};
  KAnonKeyReportingIDParam id_param_2 = {
      .buyer_reporting_id = "buyer_id",
      .buyer_and_seller_reporting_id = "buyer_seller_id",
      .selected_buyer_and_seller_reporting_id = "selected_id"};
  EXPECT_EQ(hash_util_.HashedKAnonKeyForAdRenderURL(
                /*owner=*/"https://example.org",
                /*bidding_url=*/"https://example.org/bid.js",
                /*render_url=*/"https://ad.com"),
            hash_util_.HashedKAnonKeyForAdRenderURL(
                /*owner=*/"https://example.org",
                /*bidding_url=*/"https://example.org/bid.js",
                /*render_url=*/"https://ad.com"));
  EXPECT_EQ(hash_util_.HashedKAnonKeyForAdComponentRenderURL(
                /*component_render_url=*/"https://ad.com"),
            hash_util_.HashedKAnonKeyForAdComponentRenderURL(
                /*component_render_url=*/"https://ad.com"));
  EXPECT_EQ(hash_util_.HashedKAnonKeyForReportingID(
                /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
                /*bidding_url=*/"https://example.org/bid.js",
                /*render_url=*/"https://ad.com", id_param_1),
            hash_util_.HashedKAnonKeyForReportingID(
                /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
                /*bidding_url=*/"https://example.org/bid.js",
                /*render_url=*/"https://ad.com", id_param_1));

  EXPECT_NE(hash_util_.HashedKAnonKeyForAdRenderURL(
                /*owner=*/"https://example.org",
                /*bidding_url=*/"https://example.org/bid.js",
                /*render_url=*/"https://ad1.com"),
            hash_util_.HashedKAnonKeyForAdRenderURL(
                /*owner=*/"https://example.org",
                /*bidding_url=*/"https://example.org/bid.js",
                /*render_url=*/"https://ad2.com"));
  EXPECT_NE(hash_util_.HashedKAnonKeyForAdComponentRenderURL(
                /*component_render_url=*/"https://ad1.com"),
            hash_util_.HashedKAnonKeyForAdComponentRenderURL(
                /*component_render_url=*/"https://ad2.com"));
  EXPECT_NE(hash_util_.HashedKAnonKeyForReportingID(
                /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
                /*bidding_url=*/"https://example.org/bid.js",
                /*render_url=*/"https://ad1.com", id_param_1),
            hash_util_.HashedKAnonKeyForReportingID(
                /*owner=*/"https://example.org", /*ig_name=*/"ig_one",
                /*bidding_url=*/"https://example.org/bid.js",
                /*render_url=*/"https://ad2.com", id_param_2));
}

}  // namespace
}  // namespace privacy_sandbox::bidding_auction_servers
