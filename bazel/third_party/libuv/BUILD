UV_COMMON_HEADERS = [
    "include/uv.h",
    "include/uv/errno.h",
    "include/uv/threadpool.h",
    "include/uv/version.h"
]

UV_COMMON_SOURCES = [
  "src/fs-poll.c",
  "src/heap-inl.h",
  "src/idna.c",
  "src/idna.h",
  "src/inet.c",
  "src/queue.h",
  "src/strscpy.c",
  "src/strscpy.h",
  "src/threadpool.c",
  "src/timer.c",
  "src/uv-data-getter-setters.c",
  "src/uv-common.c",
  "src/uv-common.h",
  "src/version.c"
]

UV_WINDOWS_SOURCES = [
    "src/win/async.c",
    "src/win/atomicops-inl.h",
    "src/win/core.c",
    "src/win/detect-wakeup.c",

    "src/win/error.c",
    "src/win/fs-event.c",
    "src/win/fs.c",
    "src/win/getaddrinfo.c",
    "src/win/getnameinfo.c",
    "src/win/handle.c",
    "src/win/handle-inl.h",
    "src/win/internal.h",
    "src/win/loop-watcher.c",
    "src/win/pipe.c",
    "src/win/poll.c",
    "src/win/process-stdio.c",
    "src/win/process.c",
    "src/win/req-inl.h",
    "src/win/signal.c",
    "src/win/stream.c",
    "src/win/stream-inl.h",
    "src/win/tcp.c",
    "src/win/thread.c",
    "src/win/tty.c",
    "src/win/udp.c",
    "src/win/util.c",
    "src/win/winapi.c",
    "src/win/winapi.h",
    "src/win/winsock.c",
    "src/win/winsock.h"
]

UV_WINDOWS_HEADERS = [
    "include/uv/win.h",
    "include/uv/tree.h"
]

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

  "src/unix/fs.c",
  "src/unix/getaddrinfo.c",
  "src/unix/getnameinfo.c",
  "src/unix/internal.h",
  "src/unix/loop-watcher.c",
  "src/unix/loop.c",
  "src/unix/pipe.c",
  "src/unix/poll.c",
  "src/unix/process.c",
  "src/unix/signal.c",
  "src/unix/spinlock.h",
  "src/unix/stream.c",
  "src/unix/tcp.c",
  "src/unix/thread.c",
  "src/unix/tty.c",
  "src/unix/udp.c"
]

UV_UNIX_HEADERS = [
    "include/uv/unix.h"
]

UV_DARWIN_SOURCES = [
  "src/unix/bsd-ifaddrs.c",
  "src/unix/darwin.c",

  "src/unix/fsevents.c",
  "src/unix/kqueue.c",
  "src/unix/proctitle.c",
]

UV_DARWIN_HEADERS = [
    "include/uv/darwin.h"
]

UV_LINUX_SOURCES = [
  "src/unix/linux-core.c",
  "src/unix/linux-inotify.c",
  "src/unix/linux-syscalls.c",
  "src/unix/linux-syscalls.h",
  "src/unix/procfs-exepath.c",
  "src/unix/proctitle.c",
  "src/unix/sysinfo-loadavg.c"
]

UV_LINUX_HEADERS = [
    "include/uv/linux.h"
]

cc_library(
  name = "libuv",
  srcs = UV_COMMON_SOURCES + select({
    "@bazel_tools//src/conditions:darwin": UV_UNIX_SOURCES + UV_DARWIN_SOURCES,
    "@bazel_tools//src/conditions:windows": UV_WINDOWS_SOURCES,
    "//conditions:default": UV_UNIX_SOURCES + UV_LINUX_SOURCES,
  }),
  hdrs = UV_COMMON_HEADERS + select({
    "@bazel_tools//src/conditions:darwin": UV_UNIX_HEADERS + UV_DARWIN_HEADERS,
    "@bazel_tools//src/conditions:windows": UV_WINDOWS_HEADERS,
    "//conditions:default": UV_UNIX_HEADERS + UV_LINUX_HEADERS,
  }),
  linkopts = select({
      "@bazel_tools//src/conditions:windows": WIN_LINK_OPTS,
      "//conditions:default": [],
  }),
  includes = ["include", "src"],
  visibility = ["//visibility:public"],
)