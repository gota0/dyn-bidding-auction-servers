# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "runtime_flags",
    hdrs = [
        "runtime_flags.h",
    ],
    deps = [
        "//services/common/constants:common_service_flags",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "seller_frontend_service",
    srcs = [
        "select_ad_reactor.cc",
        "select_ad_reactor_app.cc",
        "select_ad_reactor_invalid_client.cc",
        "select_ad_reactor_web.cc",
        "seller_frontend_service.cc",
    ],
    hdrs = [
        "select_ad_reactor.h",
        "select_ad_reactor_app.h",
        "select_ad_reactor_invalid_client.h",
        "select_ad_reactor_web.h",
        "seller_frontend_service.h",
    ],
    deps = [
        ":runtime_flags",
        ":seller_frontend_providers",
        "//api:bidding_auction_servers_cc_grpc_proto",
        "//api:bidding_auction_servers_cc_proto",
        "//services/common/clients/auction_server:async_client",
        "//services/common/clients/buyer_frontend_server:buyer_frontend_async_client",
        "//services/common/clients/buyer_frontend_server:buyer_frontend_async_client_factory",
        "//services/common/clients/config:config_client",
        "//services/common/clients/http:multi_curl_http_fetcher_async",
        "//services/common/compression:gzip",
        "//services/common/concurrent:local_cache",
        "//services/common/constants:user_error_strings",
        "//services/common/loggers:build_input_process_response_benchmarking_logger",
        "//services/common/metric:server_definition",
        "//services/common/reporters:async_reporter",
        "//services/common/util:context_logger",
        "//services/common/util:error_accumulator",
        "//services/common/util:error_reporter",
        "//services/common/util:reporting_util",
        "//services/common/util:request_metadata",
        "//services/common/util:request_response_constants",
        "//services/common/util:scoped_cbor",
        "//services/seller_frontend_service/util:framing_utils",
        "//services/seller_frontend_service/util:startup_param_parser",
        "//services/seller_frontend_service/util:web_utils",
        "@aws_sdk_cpp//:core",
        "@com_github_google_glog//:glog",
        "@com_github_google_quiche//quiche:oblivious_http_unstable_api",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/synchronization",
        "@control_plane_shared//cc/public/cpio/interface:cpio",
        "@google_privacysandbox_servers_common//src/cpp/communication:encoding_utils",
        "@google_privacysandbox_servers_common//src/cpp/communication:ohttp_utils",
        "@google_privacysandbox_servers_common//src/cpp/concurrent:executor",
        "@google_privacysandbox_servers_common//src/cpp/encryption/key_fetcher/interface:key_fetcher_manager_interface",
        "@google_privacysandbox_servers_common//src/cpp/encryption/key_fetcher/src:key_fetcher_manager",
        "@google_privacysandbox_servers_common//src/cpp/telemetry",
        "@libcbor//:cbor",
    ],
)

cc_library(
    name = "seller_frontend_data",
    hdrs = [
        "data/scoring_signals.h",
    ],
    deps = [
        "//api:bidding_auction_servers_cc_grpc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_library(
    name = "seller_frontend_providers",
    srcs = [
        "providers/http_scoring_signals_async_provider.cc",
    ],
    hdrs = [
        "providers/http_scoring_signals_async_provider.h",
        "providers/scoring_signals_async_provider.h",
    ],
    deps = [
        ":seller_frontend_data",
        "//services/common/clients:seller_key_value_async_http_client",
        "//services/common/providers:async_provider",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/status",
    ],
)

cc_test(
    name = "seller_frontend_providers_test",
    size = "small",
    srcs = [
        "providers/http_scoring_signals_async_provider_test.cc",
    ],
    deps = [
        "seller_frontend_providers",
        "//services/common/test:mocks",
        "//services/common/test:random",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_binary(
    name = "server",
    srcs = ["seller_frontend_main.cc"],
    linkopts = [
        "-Wl,-rpath,\\$$ORIGIN/../lib",
    ],
    deps = [
        ":seller_frontend_service",
        "//services/common/clients/config:config_client_util",
        "//services/common/encryption:crypto_client_factory",
        "//services/common/encryption:key_fetcher_factory",
        "//services/common/telemetry:configure_telemetry",
        "//services/common/util:status_macros",
        "@com_github_google_glog//:glog",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",  # for grpc_cli
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@google_privacysandbox_servers_common//src/cpp/telemetry",
        "@rapidjson",
    ],
)

cc_test(
    name = "select_ad_reactor_test",
    size = "small",
    srcs = [
        "select_ad_reactor_test.cc",
    ],
    deps = [
        ":seller_frontend_service",
        "//services/common/test:mocks",
        "//services/common/test:random",
        "//services/common/test/utils:cbor_test_utils",
        "//services/seller_frontend_service/util:select_ad_reactor_test_utils",
        "//services/seller_frontend_service/util:web_utils",
        "@com_google_googletest//:gtest_main",
        "@google_privacysandbox_servers_common//src/cpp/encryption/key_fetcher/mock:mock_key_fetcher_manager",
    ],
)

cc_test(
    name = "select_ad_reactor_web_test",
    size = "small",
    srcs = [
        "select_ad_reactor_web_test.cc",
    ],
    deps = [
        ":seller_frontend_service",
        "//services/common/test:mocks",
        "//services/common/test:random",
        "//services/common/test/utils:cbor_test_utils",
        "//services/seller_frontend_service/util:select_ad_reactor_test_utils",
        "@com_github_google_quiche//quiche:oblivious_http_unstable_api",
        "@com_google_googletest//:gtest_main",
        "@google_privacysandbox_servers_common//src/cpp/communication:ohttp_utils",
        "@google_privacysandbox_servers_common//src/cpp/encryption/key_fetcher/mock:mock_key_fetcher_manager",
        "@libcbor//:cbor",
    ],
)

cc_test(
    name = "seller_frontend_service_test",
    size = "small",
    srcs = [
        "seller_frontend_service_test.cc",
    ],
    deps = [
        ":seller_frontend_service",
        "//services/common/test:mocks",
        "//services/common/test:random",
        "//services/common/test/utils:cbor_test_utils",
        "//services/common/test/utils:service_utils",
        "//services/seller_frontend_service/util:select_ad_reactor_test_utils",
        "@com_google_absl//absl/base:log_severity",
        "@com_google_absl//absl/log:scoped_mock_log",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
        "@google_privacysandbox_servers_common//src/cpp/encryption/key_fetcher/mock:mock_key_fetcher_manager",
    ],
)

cc_test(
    name = "select_ad_reactor_app_test",
    size = "small",
    srcs = [
        "select_ad_reactor_app_test.cc",
    ],
    deps = [
        ":seller_frontend_service",
        "//services/common/compression:gzip",
        "//services/common/test:mocks",
        "//services/common/test:random",
        "//services/seller_frontend_service/util:framing_utils",
        "//services/seller_frontend_service/util:select_ad_reactor_test_utils",
        "@com_github_google_quiche//quiche:oblivious_http_unstable_api",
        "@com_google_googletest//:gtest_main",
        "@google_privacysandbox_servers_common//src/cpp/communication:encoding_utils",
        "@google_privacysandbox_servers_common//src/cpp/communication:ohttp_utils",
        "@google_privacysandbox_servers_common//src/cpp/encryption/key_fetcher/mock:mock_key_fetcher_manager",
    ],
)