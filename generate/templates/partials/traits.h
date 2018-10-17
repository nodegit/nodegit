class {{ cppClassName }};

struct {{ cppClassName }}Traits {
  typedef {{ cppClassName }} cppClass;
  typedef {{ cType }} cType;

  static const bool isDuplicable = {{ dupFunction|toBool |or cpyFunction|toBool}};
  static void duplicate({{ cType }} **dest, {{ cType }} *src) {
  {% if dupFunction %}
    {{ dupFunction }}(dest, src);
  {% elsif cpyFunction %}
    {{ cType }} *copy = ({{ cType }} *)malloc(sizeof({{ cType }}));
    {{ cpyFunction }}(copy, src);
    *dest = copy;
  {% else %}
    Nan::ThrowError("duplicate called on {{ cppClassName }} which cannot be duplicated");
  {% endif %}
  }

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
    Nan::ThrowError("free called on {{ cppClassName }} which cannot be freed");
  {% endif %}
  }
};
