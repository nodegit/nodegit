const { spawn } = require('child_process');

const [, , cmd, ...args] = process.argv;
if (!cmd) {
  process.exit(-1);
}

const once = (fn) => {
  let runOnce = false;
  return (...args) => {
    if (runOnce) {
      return;
    }

    runOnce = true;
    fn(...args);
  }
};

const retry = (numRetries = 3) => {
  const child = spawn(cmd, args, {
    shell: process.platform === 'win32',
    stdio: [0, 'pipe', 'pipe']
  });

  child.setMaxListeners(0);

  child.stdout.setEncoding('utf8');
  child.stderr.setEncoding('utf8');

  child.stdout.pipe(process.stdout);
  child.stderr.pipe(process.stderr);

  const cleanupAndExit = once((error, status) => {
    child.kill();
    if (numRetries > 0 && (error || status !== 0)) {
      retry(numRetries - 1);
    } else if (error) {
      console.log(error);
      process.exit(-1);
    } else {
      process.exit(status);
    }
  });
  const onClose = status => cleanupAndExit(null, status);

  child.on('close', onClose);
  child.on('error', cleanupAndExit);
};

retry();
