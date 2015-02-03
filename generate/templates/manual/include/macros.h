#ifndef NODEGIT_PSUEDONAN_H_
#if (NODE_MODULE_VERSION > 0x000B)
// Node 0.11+ (0.11.3 and below won't compile with these)
# define NodeGitPsueodoNanReturnEscapingValue(val) NanReturnValue(NanEscapeScope(val))

#else
// Node 0.8 and 0.10
# define NodeGitPsueodoNanReturnEscapingValue(val) NanReturnValue(val)

#endif

#endif //NODEGIT_PSUEDONAN_H_
