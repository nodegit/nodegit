const vfs = require("vinyl-fs");
const eslint = require("gulp-eslint");
const reporter = require("gulp-reporter");
vfs.src([
  "**/*.js",
  "!node_modules/**/*",
  "!dist/**/*",
  "!test/repos/**/*"
])
  .pipe(eslint())
  .pipe(reporter({
    // fail only error write before 2017-7-31
    expires: new Date("2017-7-31"),
    author: null,
  })).resume().on("error", (e) => {
    console.error(String(e))
    process.exitCode = -1
  });
