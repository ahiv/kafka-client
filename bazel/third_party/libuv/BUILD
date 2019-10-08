UV_COMMON_SOURCES = [
  "include/uv/unix.h",
  "include/uv/linux.h",
  "include/uv/sunos.h",
  "include/uv/darwin.h",
  "include/uv/bsd.h",
  "include/uv/aix.h",
  "src/fs-poll.c",
  "src/heap-inl.h",
  "src/strscpy.c",
  "src/strscpy.h",
  "src/inet.c",
  "src/idna.c",
  "src/queue.h",
  "src/threadpool.c",
  "src/timer.c",
  "src/uv-common.c",
  "src/uv-common.h",
  "src/uv-data-getter-setters.c",
  "src/version.c"
]

UV_WINDOWS_SOURCES = glob(["src/win/*"])

WIN_LINK_OPTS = [
    "-DEFAULTLIB:advapi32.lib",
    "-DEFAULTLIB:userenv.lib",
    "-DEFAULTLIB:user32.lib",
    "-DEFAULTLIB:Iphlpapi.lib",
    "-DEFAULTLIB:ws2_32.lib",
]

UV_UNIX_SOURCES = [
  "src/unix/async.c",
  "src/unix/atomic-ops.h",
  "src/unix/core.c",
  "src/unix/dl.c",
  "src/unix/fs.c",
  "src/unix/getaddrinfo.c",
  "src/unix/getnameinfo.c",
  "src/unix/internal.h",
  "src/unix/loop.c",
  "src/unix/loop-watcher.c",
  "src/unix/pipe.c",
  "src/unix/poll.c",
  "src/unix/process.c",
  "src/unix/signal.c",
  "src/unix/spinlock.h",
  "src/unix/stream.c",
  "src/unix/tcp.c",
  "src/unix/thread.c",
  "src/unix/tty.c",
  "src/unix/udp.c",
]

UV_DARWIN_SOURCES = [
  "src/unix/bsd-ifaddrs.c",
  "src/unix/darwin.c",
  "src/unix/darwin-proctitle.c",
  "src/unix/fsevents.c",
  "src/unix/kqueue.c",
  "src/unix/proctitle.c",
]

UV_LINUX_SOURCES = [
    "src/unix/linux-core.c",
    "src/unix/linux-inotify.c",
    "src/unix/linux-syscalls.c",
    "src/unix/linux-syscalls.h",
    "src/unix/procfs-exepath.c",
    "src/unix/proctitle.c",
    "src/unix/sysinfo-loadavg.c",
    "src/unix/sysinfo-memory.c",
]

cc_library(
  name = "libuv",
  srcs = UV_COMMON_SOURCES + select({
    "@bazel_tools//src/conditions:darwin": UV_UNIX_SOURCES + UV_DARWIN_SOURCES,
    "@bazel_tools//src/conditions:windows": UV_WINDOWS_SOURCES,
    "//conditions:default": UV_UNIX_SOURCES + UV_LINUX_SOURCES,
  }),
  hdrs = [
    "src/unix/internal.h",
    "include/uv.h",
    "include/uv/darwin.h",
    "include/uv/errno.h",
    "include/uv/threadpool.h",
    "include/uv/tree.h",
    "include/uv/unix.h",
    "include/uv/version.h",
  ],
  linkopts = select({
      "@bazel_tools//src/conditions:windows": WIN_LINK_OPTS,
      "//conditions:default": [],
  }),
  includes = ["include", "src"],
  visibility = ["//visibility:public"],
)