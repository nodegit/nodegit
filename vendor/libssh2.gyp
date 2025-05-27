{
  "variables": {
    "is_electron%": "<!(node ../utils/isBuildingForElectron.js <(node_root_dir))",
    "macOS_deployment_target": "10.11"
  },

  "targets": [
    {
      "target_name": "acquireOpenSSL",
        "type": "none",
        "conditions": [
        ["<(is_electron) == 1 and <!(node -p \"process.env.npm_config_openssl_dir ? 0 : 1\")", {
          "actions": [{
            "action_name": "acquire",
            "action": ["node", "../utils/acquireOpenSSL.mjs", "<(macOS_deployment_target)"],
            "inputs": [""],
            "outputs": ["./openssl"],
            "message": "Acquiring OpenSSL binaries and headers"
          }]
        }]
      ]
    },
    {
      "target_name": "configureLibssh2",
      "type": "none",
      "actions": [{
        "action_name": "configure",
        "action": ["node", "../utils/configureLibssh2.js"],
        "inputs": [""],
        "outputs": [""],
        "message": "Confioguring libssh2"
      }],
      "dependencies": [
        "acquireOpenSSL"
      ]
    }
  ]
}
