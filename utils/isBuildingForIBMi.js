const os = require("os");
const isIBMi = (os.platform() == "aix" && os.type() == "OS400");

process.stdout.write(isIBMi ? "1" : "0");
