class {{ cppClassName }};
{% if type == 'struct' %}
class Configurable{{ cppClassName }};
{% endif %}

struct {{ cppClassName }}Traits {
  typedef {{ cppClassName }} cppClass;
  typedef {{ cType }} cType;
  {% if type == 'struct' %}
  typedef Configurable{{ cppClassName }} configurableCppClass;
  {% endif %}

  static const bool isDuplicable = {{ dupFunction|toBool |or cpyFunction|toBool}};
  static void duplicate({{ cType }} **dest, {{ cType }} *src) {
  {% if dupFunction %}
    {{ dupFunction }}(dest, src);
  {% elsif cpyFunction %}
    {{ cType }} *copy = ({{ cType }} *)malloc(sizeof({{ cType }}));
    {{ cpyFunction }}(copy, src);
    *dest = copy;
  {% else %}
    Napi::Env env = nodegit::Context::GetCurrentContext()->Env();
    Napi::Error::New(env, "duplicate called on {{ cppClassName }} which cannot be duplicated").ThrowAsJavaScriptException();
    return;
  {% endif %}
  }

  static std::string className() { return "{{ cppClassName }}"; };
  static const bool isSingleton = {{ isSingleton | toBool }};
  static const bool isFreeable = {{ freeFunctionName | toBool}};
  static void free({{ cType }} *raw) {
  {% if freeFunctionName %}
    unsigned long referenceCount = 0;
    {% if isSingleton %}
    referenceCount = ReferenceCounter::decrementCountForPointer((void *)raw);
    {% endif %}
    if (referenceCount == 0) {
      ::{{ freeFunctionName }}(raw); // :: to avoid calling this free recursively
    }
  {% else %}
    Napi::Env env = nodegit::Context::GetCurrentContext()->Env();
    Napi::Error::New(env, "free called on {{ cppClassName }} which cannot be freed").ThrowAsJavaScriptException();
    return;
  {% endif %}
  }
};
