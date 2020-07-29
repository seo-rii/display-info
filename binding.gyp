{
  "targets": [
    {
      "target_name": "display-info-wrapper",
      "conditions":[
        ["OS=='win'", {
          "sources": [
            "src/main.cpp"
          ]
        }]
      ],
      "cflags": [
        "-O3"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "defines": [
        "NAPI_DISABLE_CPP_EXCEPTIONS"
      ]
    }
  ]
}