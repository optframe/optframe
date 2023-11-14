load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

package(
    default_visibility = ["//visibility:public"],
)

# location of .clang-tidy file for plugin
filegroup(
    name = "clang_tidy_config",
    srcs = [
        ".clang-tidy",
    ],
)

cc_library(
    name = "optframe",
    deps = ["//include:OptFrame"],
)

cc_library(
    name = "optfcore",
    deps = ["//include:OptFCore"],
)