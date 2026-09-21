Napi::Value GitLibgit2::Opts(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();

  if (info.Length() == 0 || !info[0].IsNumber()) {
    Napi::Error::New(env, "Number option is required.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  const int from_option = (int)info[0].As<Napi::Number>().Int32Value();

  git_error_clear();

  Napi::Value to = env.Undefined();
  switch (from_option) {
    // GET size_t
    case GIT_OPT_GET_MWINDOW_SIZE:
    case GIT_OPT_GET_MWINDOW_MAPPED_LIMIT:
    case GIT_OPT_GET_PACK_MAX_OBJECTS: {
      size_t option_value;
      if (git_libgit2_opts(from_option, &option_value)) {
        Napi::Error::New(env, "git_libgit2_opts failed").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      to = Napi::Number::New(env, option_value);
      break;
    }
    // GET int
    case GIT_OPT_GET_OWNER_VALIDATION: {
      int option_value;
      if (git_libgit2_opts(from_option, &option_value)) {
        Napi::Error::New(env, "git_libgit2_opts failed").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      to = Napi::Number::New(env, option_value);
      break;
    }
    // GET unsigned long
    case GIT_OPT_GET_WINDOWS_SHAREMODE: {
      unsigned long option_value;
      if (git_libgit2_opts(from_option, &option_value)) {
        Napi::Error::New(env, "git_libgit2_opts failed").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      to = Napi::Number::New(env, option_value);
      break;
    }
    // GET ssize_t
    case GIT_OPT_GET_CACHED_MEMORY: {
      ssize_t option_value;
      if (git_libgit2_opts(from_option, &option_value)) {
        Napi::Error::New(env, "git_libgit2_opts failed").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      to = Napi::Number::New(env, option_value);
      break;
    }
    // GET git_buf
    case GIT_OPT_GET_TEMPLATE_PATH:
    case GIT_OPT_GET_USER_AGENT: {
      git_buf option_value = { 0 };
      if (git_libgit2_opts(from_option, &option_value)) {
        Napi::Error::New(env, "git_libgit2_opts failed").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      to = Napi::String::New(env, option_value.ptr, option_value.size);
      git_buf_dispose(&option_value);
      break;
    }
    case GIT_OPT_GET_SEARCH_PATH: {
      git_buf option_value = { 0 };
      if (info.Length() < 2 || !info[1].IsNumber()) {
        Napi::Error::New(env, "Number option is required.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      const int level = (int)info[1].As<Napi::Number>().Int32Value();
      if (git_libgit2_opts(from_option, level, &option_value)) {
        Napi::Error::New(env, "git_libgit2_opts failed").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      to = Napi::String::New(env, option_value.ptr, option_value.size);
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
      if (info.Length() < 2 || !info[1].IsNumber()) {
        Napi::Error::New(env, "Number option is required.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      const int option_arg = (int)info[1].As<Napi::Number>().Int32Value();
      if (git_libgit2_opts(from_option, option_arg)) {
        Napi::Error::New(env, "git_libgit2_opts failed").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      break;
    }
    // SET size_t
    case GIT_OPT_SET_MWINDOW_SIZE:
    case GIT_OPT_SET_MWINDOW_MAPPED_LIMIT:
    case GIT_OPT_SET_PACK_MAX_OBJECTS: {
      if (info.Length() < 2 || !info[1].IsNumber()) {
        Napi::Error::New(env, "Number option is required.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      const size_t option_arg = (size_t)info[1].As<Napi::Number>().DoubleValue();
      if (git_libgit2_opts(from_option, option_arg)) {
        Napi::Error::New(env, "git_libgit2_opts failed").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      break;
    }
    default: {
      Napi::Error::New(env, "Unsupported option").ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }

  return to;
}
