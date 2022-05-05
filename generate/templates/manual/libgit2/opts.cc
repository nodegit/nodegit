NAN_METHOD(GitLibgit2::Opts)
{
  Nan::EscapableHandleScope scope;

  if (info.Length() == 0 || !info[0]->IsNumber()) {
    return Nan::ThrowError("Number option is required.");
  }

  const int from_option = (int)info[0].As<v8::Number>()->Value();

  git_error_clear();

  v8::Local<v8::Value> to = Nan::Undefined();
  switch (from_option) {
    // GET size_t
    case GIT_OPT_GET_MWINDOW_SIZE:
    case GIT_OPT_GET_MWINDOW_MAPPED_LIMIT:
    case GIT_OPT_GET_PACK_MAX_OBJECTS: {
      size_t option_value;
      if (git_libgit2_opts(from_option, &option_value)) {
        return Nan::ThrowError("git_libgit2_opts failed");
      }
      to = Nan::New<Number>(option_value);
      break;
    }
    // GET int
    case GIT_OPT_GET_OWNER_VALIDATION: {
      int option_value;
      if (git_libgit2_opts(from_option, &option_value)) {
        return Nan::ThrowError("git_libgit2_opts failed");
      }
      to = Nan::New<Number>(option_value);
      break;
    }
    // GET unsigned long
    case GIT_OPT_GET_WINDOWS_SHAREMODE: {
      unsigned long option_value;
      if (git_libgit2_opts(from_option, &option_value)) {
        return Nan::ThrowError("git_libgit2_opts failed");
      }
      to = Nan::New<Number>(option_value);
      break;
    }
    // GET ssize_t
    case GIT_OPT_GET_CACHED_MEMORY: {
      ssize_t option_value;
      if (git_libgit2_opts(from_option, &option_value)) {
        return Nan::ThrowError("git_libgit2_opts failed");
      }
      to = Nan::New<Number>(option_value);
      break;
    }
    // GET git_buf
    case GIT_OPT_GET_TEMPLATE_PATH:
    case GIT_OPT_GET_USER_AGENT: {
      git_buf option_value = { 0 };
      if (git_libgit2_opts(from_option, &option_value)) {
        return Nan::ThrowError("git_libgit2_opts failed");
      }
      to = Nan::New<v8::String>(option_value.ptr, option_value.size)
             .ToLocalChecked();
      git_buf_dispose(&option_value);
      break;
    }
    case GIT_OPT_GET_SEARCH_PATH: {
      git_buf option_value = { 0 };
      if (info.Length() < 2 || !info[1]->IsNumber()) {
        return Nan::ThrowError("Number option is required.");
      }
      const int level = (int)info[1].As<v8::Number>()->Value();
      if (git_libgit2_opts(from_option, level, &option_value)) {
        return Nan::ThrowError("git_libgit2_opts failed");
      }
      to = Nan::New<v8::String>(option_value.ptr, option_value.size)
             .ToLocalChecked();
      git_buf_dispose(&option_value);
      break;
    }
    // SET int
    case GIT_OPT_ENABLE_CACHING:
    case GIT_OPT_ENABLE_STRICT_OBJECT_CREATION:
    case GIT_OPT_ENABLE_STRICT_SYMBOLIC_REF_CREATION:
    case GIT_OPT_ENABLE_OFS_DELTA:
    case GIT_OPT_ENABLE_FSYNC_GITDIR:
    case GIT_OPT_ENABLE_STRICT_HASH_VERIFICATION:
    case GIT_OPT_ENABLE_UNSAVED_INDEX_SAFETY:
    case GIT_OPT_DISABLE_PACK_KEEP_FILE_CHECKS:
    case GIT_OPT_SET_OWNER_VALIDATION: {
      if (info.Length() < 2 || !info[1]->IsNumber()) {
        return Nan::ThrowError("Number option is required.");
      }
      const int option_arg = (int)info[1].As<v8::Number>()->Value();
      if (git_libgit2_opts(from_option, option_arg)) {
        return Nan::ThrowError("git_libgit2_opts failed");
      }
      break;
    }
    // SET size_t
    case GIT_OPT_SET_MWINDOW_SIZE:
    case GIT_OPT_SET_MWINDOW_MAPPED_LIMIT:
    case GIT_OPT_SET_PACK_MAX_OBJECTS: {
      if (info.Length() < 2 || !info[1]->IsNumber()) {
        return Nan::ThrowError("Number option is required.");
      }
      const size_t option_arg = (size_t)info[1].As<v8::Number>()->Value();
      if (git_libgit2_opts(from_option, option_arg)) {
        return Nan::ThrowError("git_libgit2_opts failed");
      }
      break;
    }
    default: {
      return Nan::ThrowError("Unsupported option");
    }
  }

  return info.GetReturnValue().Set(scope.Escape(to));
}
