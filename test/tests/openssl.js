var path = require("path");
var assert = require("assert");
var subprocess = require("child_process");

/***
 * Ideally, we would have opportunity to provide input argument(s) in
 * addition to process.argv input (see examples); however, we can
 * still unit-test.
 *
 * @example
 * it("acquiring openssl returns void upon call", async () => {
 *   var { acquireOpenSSL } = require("../../utils/acquireOpenSSL");
 *   // Expected outcome is void (null);
 *   const response = await acquireOpenSSL();
 *   assert.equal(response, void null);
 * });
 *
 * @example
 * it("downloading openssl returns void upon call", async () => {
 *   var { downloadOpenSSLIfNecessary } = require("../../utils/acquireOpenSSL");
 *   // Expected outcome is void (null);
 *   const response = await downloadOpenSSLIfNecessary();
 *   assert.equal(response, void null);
 * });
 *
 * @example
 * it("building openssl returns void upon call", async () => {
 *   var { buildOpenSSLIfNecessary } = require("../../utils/acquireOpenSSL");
 *   // Expected outcome is void (null);
 *   const response = await buildOpenSSLIfNecessary();
 *   assert.equal(response, void null);
 * });
 */

describe("OpenSSL", function () {
  /***
   * @note - darwin 10.15, v1.1.1l builds are failing. However, v1.1.1p are succeeding, so it's defined as 1.1.1p, and is
   * set to `skip` until project maintainers can elect to either remove, use, or evaluate for another time.
   *
   * @note - the {"macOS_deployment_target": "10.11"} is currently the target in the gyp binding(s) file;
   */
  it.skip( "darwin 10.15, openSSL v1.1.1p acquireOpenSSL successfully executes", function () {
    function execute () {
      try {
        subprocess.execFileSync("node", [ path.join(process.cwd(), "utils", "acquireOpenSSL.js"), "10.15" ], {
          shell: false, encoding: "utf-8", env: process.env, cwd: process.cwd(), stdio: "ignore"
        });

        return 0;
      } catch (exception) {
        console.trace(exception);

        return 1;
      }
    }

    assert(execute() === 0);
  });
});
