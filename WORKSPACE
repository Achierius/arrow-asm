workspace(name = "beautiful-asm")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "rules_cc",
    sha256 = "76737f9070decb43c9f1c6f1f7c4e40555de09f4312ad7e63f4f4727911d1478",
    strip_prefix = "rules_cc-9ec8187d589e7554e8cfe8d14b3917ebe4b94940",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/9ec8187d589e7554e8cfe8d14b3917ebe4b94940.tar.gz"],
)

http_archive(
    name = "com_github_fmtlib_fmt",
    build_file = "//foreign:fmt.BUILD",
    sha256 = "335b41fe614a35610ab92f8c1525e57cdbde75a72f2eb8e84949e4c25d8c31f8",
    strip_prefix = "fmt-d9f045fba18c6897ae0931a931450638560e3fd4",
    urls = ["https://github.com/fmtlib/fmt/archive/d9f045fba18c6897ae0931a931450638560e3fd4.tar.gz"],
)

http_archive(
    name = "com_github_gabime_spdlog",
    build_file = "//foreign:spdlog.BUILD",
    sha256 = "d00ccce646e39286d7ca9c45ca13b22828e1a63ed5a2cb6769db936f1e2b1668",
    strip_prefix = "spdlog-729d7f6d8837b6693e7b378408518ea1710f80cb",
    urls = ["https://github.com/gabime/spdlog/archive/729d7f6d8837b6693e7b378408518ea1710f80cb.tar.gz"],
)

