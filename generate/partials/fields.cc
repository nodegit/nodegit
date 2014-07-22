{%each fields as field%}
  {%if not field.ignore%}

NAN_METHOD({{ cppClassName }}::{{ field.cppFunctionName }}) {
  NanScope();
  <% var to = fieldInfo; -%>
  Handle<Value> to;

  <%- fieldInfo.cType %> <% if (!isV8Value(fieldInfo.cppClassName)) { %>*<% } %><%- fieldInfo.name %> =
    <% if (!isV8Value(fieldInfo.cppClassName)) { %>&<% } %>ObjectWrap::Unwrap<<%- cppClassName %>>(args.This())->GetValue()-><%- fieldInfo.name %>;

  <% include convertToV8.cc.ejs -%>
  NanReturnValue(to);
}
<% } -%>
<% } -%>
