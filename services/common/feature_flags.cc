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

#include "feature_flags.h"

#include <optional>

#include "absl/flags/flag.h"

ABSL_FLAG(bool, enable_temporary_unlimited_egress, false,
          "If true, temporary unlimited egress is allowed (if "
          "client had also set enable_unlimited_egress flag in request)");

ABSL_FLAG(bool, enable_chaffing, false,
          "If true, chaff requests are sent out from the SFE. Chaff requests "
          "are requests sent to buyers not participating in an auction to mask "
          "the buyers associated with a client request.");
