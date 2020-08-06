// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "absl/time/clock.h"
#include "iree/base/api.h"

IREE_API_EXPORT iree_time_t iree_time_now() {
  // TODO(#2801): replace with code from //w/l/v/ to drop dep on abseil.
  return absl::GetCurrentTimeNanos();
}
