var NodeGit = require("../");

var Mempack = NodeGit.Mempack;

// Mempack.prototype.reset() is directly generated from git_mempack_reset.
// No additional JS-level customizations needed.
//
// Ownership lifecycle:
//   1. Create via Mempack.create() - allocates a git_odb_backend*
//   2. Attach to an ODB via odb.addMempackBackend(backend, priority)
//      After this, libgit2 owns the backend pointer.
//   3. Use the ODB normally (write blobs, trees, etc.)
//   4. Call backend.reset() to clear in-memory objects (ODB must still be alive)
//   Do NOT call reset() after the ODB/repo has been freed.
//   Do NOT add the same backend to multiple ODBs.
