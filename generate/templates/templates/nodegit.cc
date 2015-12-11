// This is a generated file, modify: generate/templates/nodegit.cc.
#include <v8.h>
#include <node.h>
#include <git2.h>

#include "../include/nodegit.h"
#include "../include/wrapper.h"
#include "../include/functions/copy.h"
{% each %}
  {% if type != "enum" %}
    #include "../include/{{ filename }}.h"
  {% endif %}
{% endeach %}

void run_libgit2_event_queue(void *);

extern "C" void init(Local<v8::Object> target) {
  Nan::HandleScope scope;

  Wrapper::InitializeComponent(target);
  {% each %}
    {% if type != "enum" %}
      {{ cppClassName }}::InitializeComponent(target);
    {% endif %}
  {% endeach %}

  // initialize a thread on which we will execute all libgit2 calls
  // for async NodeGit wrappers (and sync ones, eventually)
  libgit2_loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
  uv_loop_init(libgit2_loop);

  uv_thread_t libgit2_thread_id;
  uv_thread_create(&libgit2_thread_id, run_libgit2_event_queue, NULL);
}

NODE_MODULE(nodegit, init)

uv_loop_t *libgit2_loop;

void run_libgit2_event_queue(void *)
{
  // run, CPU, run
  for ( ; ; ) {
    uv_run(libgit2_loop, UV_RUN_DEFAULT);
    // TODO: break out of loop
  }

  uv_loop_close(libgit2_loop);
  free(libgit2_loop);
}
