const path = require("path");
const combyne = require("combyne");
const file = require("./util/file");
const idefs = require("./idefs");

// Customize the delimiters so as to not process `{{{` or `}}}`.
combyne.settings.delimiters = {
  START_RAW: "{{=",
  END_RAW: "=}}"
};

var partial = {
  asyncFunction: file.read("partials/async_function.cc"),
  convertFromV8: file.read("partials/convert_from_v8.cc"),
  convertToV8: file.read("partials/convert_to_v8.cc"),
  doc: file.read("partials/doc.cc"),
  fields: file.read("partials/fields.cc"),
  guardArguments: file.read("partials/guard_arguments.cc"),
  syncFunction: file.read("partials/sync_function.cc")
};

var template = {
  class: file.read("templates/class.cc"),
  //fs.readFileSync(local("templates/header.h")),
  //fs.readFileSync(local("templates/binding.gyp")),
  binding: file.read("templates/binding.gyp"),
  nodegit: file.read("templates/nodegit.cc")
};

// Convert Buffers to Combyne templates.
Object.keys(template).forEach(function(name) {
  template[name] = combyne(template[name]);
});

// Attach all partials to select templates.
Object.keys(partial).forEach(function(name) {
  template.class.registerPartial(name, combyne(partial[name]));
});

// Determine which definitions to actually include in the source code.
var enabled = idefs.filter(function(idef) {
  // Additionally provide a friendly name to the actual filename.
  idef.name = path.basename(idef.filename, ".h");

  return !idef.ignore;
});

// TODO Wipe out all existing source files.

// Write out single purpose templates.
file.write("../binding.gyp", template.binding.render(enabled));
file.write("../src/nodegit.cc", template.nodegit.render(enabled));

// Write out all the classes.
enabled.forEach(function(idef) {
  file.write("../src/" + idef.name + ".cc", template.class.render(idef));
});
