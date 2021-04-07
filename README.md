Build CGO enabled darwin (mac) binaries from linux using [GoReleaser Action](https://github.com/goreleaser/goreleaser-action).
<hr>

Your `.github/workflows/release.yml` should include this:
```yml
-
  name: OSXCross for CGO Support
  run: |
    mkdir ../../osxcross
    git clone https://github.com/plentico/osxcross-target.git ../../osxcross/target
```

<details>
<summary>Note on libcrypto (OpenSSL)</summary>
<br>
  
We updated this project for Ubuntu 20.04, but OSXCross still needs an older version of OpenSSL so you might need to add a step that looks like this:
```yml
-
  name: Downgrade libssl
  run: |
    echo 'deb http://security.ubuntu.com/ubuntu bionic-security main' | sudo tee -a /etc/apt/sources.list
    sudo apt update && apt-cache policy libssl1.0-dev
    sudo apt-get install libssl1.0-dev
```

See more details here: https://github.com/plentico/osxcross-target/issues/1

</details>

Your `.goreleaser.yml` should have separate builds for linux and mac like this:
```yml
builds:
- id: linux-build
  env:
  - CGO_ENABLED=1
  goos:
  - linux
  ignore:
  - goos: linux
    goarch: 386
- id: darwin-build
  ldflags:
  - -s
  env:
  - CGO_ENABLED=1
  - CC=/home/runner/work/osxcross/target/bin/o64-clang
  - CXX=/home/runner/work/osxcross/target/bin/o64-clang++
  goos:
  - darwin
  ignore:
  - goos: darwin
    goarch: 386
```

**Note:** the values for `CC` and `CXX` must be exactly what is referenced above because the paths are hardcoded.

<details>
<summary>Instructions for creating your own OSXCross target</summary>
<br>

Steps to create an [OSXCross](https://github.com/tpoechtrager/osxcross
) target:

1. Download OSXCross: https://github.com/tpoechtrager/osxcross/archive/master.zip
2. Unzip the downloaded archive from step 1 so the project lives at `~/Downloads/osxcross-master/`
3. Login and download XCode from https://developer.apple.com/download/more/ (version 11.6 worked for me, the beta release for 12 did not)
4. Move the downloaded archive from step 3 so it lives at `~/Downloads/osxcross-master/Xcode_11.6.xip`
5. Move into the OSXCross folder (`cd ~/Downloads/osxcross-master`) and run `./tools/gen_sdk_package_pbzx.sh Xcode_11.6.xip` which creates the SDK that lives at `~/Downloads/osxcross-master/MacOSX10.15.sdk.tar.xz`. **Note:** I would have loved to used a precompiled SDK from https://github.com/phracker/MacOSX-SDKs but using those gave me the following errors:
```
# crypto/x509
osxcross: error: cannot find libc++ headers
osxcross: error: while detecting target
# rogchap.com/v8go
In file included from v8go.cc:3:
../../../../go/pkg/mod/rogchap.com/v8go@v0.2.0/deps/include/v8.h:23:10: fatal error: 'type_traits' file not found
```
6. Move the SDK created in step 5 into the "tarballs" directory: `mv ~/Downloads/osxcross-master/MacOSX10.15.sdk.tar.xz ~/Downloads/osxcross-master/tarballs/MacOSX10.15.sdk.tar.xz`
7. While still in `~/Downloads/osxcross-master/` run the build: `./build.sh`. That should create a "target" directory with a bunch of files and folders in it, one of which is `~/Downloads/osxcross-master/target/bin/` that contains various cross-compilers, including `o64-clang` that we'll be using. The osxcross-target project is the "target" directory that's created using a hardcoded path that can be used with GitHub Actions.

After completing the steps above, you should be able to create a darwin binary using the following command (Update the paths to use your computer's username, not jimafisk):
```
env GOOS=darwin GOARCH=amd64 CGO_ENABLED=1 CC=/home/jimafisk/Downloads/osxcross-master/target/bin/o64-clang CXX=/home/jimafisk/Downloads/osxcross-master/target/bin/o64-clang++ go build -ldflags -s
```

See more details here: https://github.com/goreleaser/goreleaser-action/issues/233

</details>
