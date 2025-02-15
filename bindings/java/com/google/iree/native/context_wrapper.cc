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

#include "bindings/java/com/google/iree/native/context_wrapper.h"

#include "iree/base/api_util.h"
#include "iree/base/logging.h"

namespace iree {
namespace java {

namespace {

std::vector<iree_vm_module_t*> GetModulesFromModuleWrappers(
    const std::vector<ModuleWrapper*>& module_wrappers) {
  std::vector<iree_vm_module_t*> modules(module_wrappers.size());
  for (int i = 0; i < module_wrappers.size(); i++) {
    modules[i] = module_wrappers[i]->module();
  }
  return modules;
}

}  // namespace

Status ContextWrapper::Create(const InstanceWrapper& instance_wrapper) {
  RETURN_IF_ERROR(
      FromApiStatus(iree_vm_context_create(instance_wrapper.instance(),
                                           IREE_ALLOCATOR_SYSTEM, &context_),
                    IREE_LOC));
  RETURN_IF_ERROR(CreateDefaultModules());
  std::vector<iree_vm_module_t*> default_modules = {hal_module_};
  return FromApiStatus(
      iree_vm_context_register_modules(context_, default_modules.data(),
                                       default_modules.size()),
      IREE_LOC);
}

Status ContextWrapper::CreateWithModules(
    const InstanceWrapper& instance_wrapper,
    const std::vector<ModuleWrapper*>& module_wrappers) {
  auto modules = GetModulesFromModuleWrappers(module_wrappers);
  RETURN_IF_ERROR(CreateDefaultModules());

  // The ordering of modules matters, so default modules need to be at the
  // beginning of the vector.
  modules.insert(modules.begin(), hal_module_);

  return FromApiStatus(iree_vm_context_create_with_modules(
                           instance_wrapper.instance(), modules.data(),
                           modules.size(), IREE_ALLOCATOR_SYSTEM, &context_),
                       IREE_LOC);
}

Status ContextWrapper::RegisterModules(
    const std::vector<ModuleWrapper*>& module_wrappers) {
  auto modules = GetModulesFromModuleWrappers(module_wrappers);
  return FromApiStatus(iree_vm_context_register_modules(
                           context_, modules.data(), modules.size()),
                       IREE_LOC);
}

Status ContextWrapper::ResolveFunction(const FunctionWrapper& function_wrapper,
                                       iree_string_view_t name) {
  return FromApiStatus(iree_vm_context_resolve_function(
                           context_, name, function_wrapper.function()),
                       IREE_LOC);
}

int ContextWrapper::id() const { return iree_vm_context_id(context_); }

ContextWrapper::~ContextWrapper() {
  iree_vm_context_release(context_);
  iree_vm_module_release(hal_module_);
  iree_hal_device_release(device_);
  iree_hal_driver_release(driver_);
}

// TODO(jennik): Also create default string and tensorlist modules.
Status ContextWrapper::CreateDefaultModules() {
  RETURN_IF_ERROR(FromApiStatus(
      iree_hal_driver_registry_create_driver(iree_make_cstring_view("vmla"),
                                             IREE_ALLOCATOR_SYSTEM, &driver_),
      IREE_LOC));
  RETURN_IF_ERROR(FromApiStatus(iree_hal_driver_create_default_device(
                                    driver_, IREE_ALLOCATOR_SYSTEM, &device_),
                                IREE_LOC));
  return FromApiStatus(
      iree_hal_module_create(device_, IREE_ALLOCATOR_SYSTEM, &hal_module_),
      IREE_LOC);
}

}  // namespace java
}  // namespace iree
