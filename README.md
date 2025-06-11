# CEF Sample

[![Linux](https://github.com/paulocoutinhox/cef-sample/actions/workflows/linux.yml/badge.svg)](https://github.com/paulocoutinhox/cef-sample/actions/workflows/linux.yml)
[![Windows](https://github.com/paulocoutinhox/cef-sample/actions/workflows/windows.yml/badge.svg)](https://github.com/paulocoutinhox/cef-sample/actions/workflows/windows.yml)
[![macOS](https://github.com/paulocoutinhox/cef-sample/actions/workflows/macos.yml/badge.svg)](https://github.com/paulocoutinhox/cef-sample/actions/workflows/macos.yml)
[![Linux ARM](https://github.com/paulocoutinhox/cef-sample/actions/workflows/linux-arm.yml/badge.svg)](https://github.com/paulocoutinhox/cef-sample/actions/workflows/linux-arm.yml)

In this sample you can learn about generate desktop applications with: CXX + CEF 3 + Vue 3.

You business logic is coded in C++ and the GUI is coded in WEB techs (HTML/CSS/JS).

This sample is using Vue 3 (https://vuejs.org/) to create a better user experience working like a local WEB application. Feel free to use your own preferred WEB framework.

CEF is Chromium Embedded Framework (https://bitbucket.org/chromiumembedded/) and is automatically downloaded when build start.

## Screenshots

<p align="center">
    <img src="extras/images/app.png?rnd=2021-06-29" alt="screenshot" style="max-width: 400px">
</p>

## Download

You can download a compiled version for your system on releases page or actions page.

## CEF Version

You can change CEF_VERSION variable inside CMakeLists.txt.

Example of valid version: `120.2.7+g4bc6a59+chromium-120.0.6099.234`.

Visit https://cef-builds.spotifycdn.com/index.html for the list of supported platforms and versions.

## Platforms

* Linux x64
* Linux armV7
* Linux arm64
* macOS x64
* macOS arm64
* Windows x86
* Windows x64
* Windows arm64

> All the builds are made by Github Actions automatically on each GIT push and a new version is generated when TAG is created.

## Windows

Windows generally detect all not signed EXE as a malware by their security system (Microsoft Defender, SmartScreen etc). This false positive detection can be ignored if you add an exclusion on Windows Security. You can add entire folder making all files and subfolders not to be treated this way.

To learn how to **add an exclusion**, see Microsoft tutorial:

https://support.microsoft.com/windows/add-an-exclusion-to-windows-security-811816c0-4dfd-af4a-47e4-c301afe13b26

You can submit a file for malware analysis on Microsoft tool checker:

https://www.microsoft.com/wdsi/filesubmission

You can read more on Microsoft developer FAQ:

https://docs.microsoft.com/windows/security/threat-protection/intelligence/developer-faq

Do buy the certificate you can search on Google about companies that sell EV Code Signing Digital Certificates or go to Digicert website:

https://www.digicert.com/signing/code-signing-certificates

Some tutorials about how to signing your EXE is here:

* https://www.digicert.com/kb/code-signing/signcode-signtool-command-line.htm
* https://www.digicert.com/kb/code-signing/code-signing-winqual.exe-file.htm

Other companies that sell EV Code Signing Certificates:

* https://www.ssl.com/certificates/ev-code-signing/
* https://shop.certum.eu/data-safety/code-signing-certificates/certum-ev-code-sigining.html
* https://www.entrust.com/digital-security/certificate-solutions/products/digital-signing/code-signing-certificates
* https://www.globalsign.com/en/code-signing-certificate/ev-code-signing-certificates
* https://sectigo.com/ssl-certificates-tls/code-signing

Other option is publish your application on Microsoft Store. See some tutorials:

* https://docs.microsoft.com/windows/apps/desktop/modernize/desktop-to-uwp-distribute
* https://docs.microsoft.com/windows/msix/packaging-tool/create-an-msix-overview
* https://docs.microsoft.com/windows/win32/win_cert/windows-certification-portal
* https://www.electronjs.org/docs/tutorial/windows-store-guide

## License

This license informations is about this personal project, not the CEF Library.

[MIT](http://opensource.org/licenses/MIT)

Copyright (c) 2021-2025, Paulo Coutinho
