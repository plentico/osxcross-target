//===-- tapi/LinkerInterfaceFile.h - TAPI File Interface --------*- C++ -*-===*\
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief API for reading TAPI files.
/// \since 1.0
///
//===----------------------------------------------------------------------===//
#ifndef TAPI_LINKER_INTERFACE_FILE_H
#define TAPI_LINKER_INTERFACE_FILE_H

#include <memory>
#include <string>
#include <tapi/Defines.h>
#include <vector>

///
/// \defgroup TAPI_LINKER_INTERFACE_FILE TAPI File APIs
/// \ingroup TAPI_CPP_API
///
/// @{
///

using cpu_type_t = int;
using cpu_subtype_t = int;

TAPI_NAMESPACE_V1_BEGIN

class PackedVersion32;
class Symbol;

///
/// \brief Defines a list of supported platforms.
/// \since 1.0
/// \deprecated 1.6
///
enum class Platform : unsigned {
  /// \brief Unknown platform
  /// \since 1.0
  Unknown = 0,

  /// \brief Mac OS X
  /// \since 1.0
  OSX = 1,

  /// \brief iOS
  /// \since 1.0
  iOS = 2,

  /// \brief watchOS
  /// \since 1.0
  watchOS = 3,

  /// \brief tvOS
  /// \since 1.0
  tvOS = 4,

  /// \brief bridgeOS
  /// \since 1.2
  bridgeOS = 5,

  /// DO NOT RENAME. This needs to be coordinated with ld64.
  /// \brief iOSMac
  /// \since 1.4
  iOSMac = 6,

  /// \brief driverKit
  /// \since 1.5
  DriverKit = 7,

  /// \brief zippered
  /// This value is temporary for bringup and will be removed.
  /// \since 1.4
  zippered = ~0U,
} __attribute__((deprecated));

///
/// \brief Defines a list of Objective-C constraints.
/// \since 1.0
///
enum class ObjCConstraint : unsigned {
  /// \brief No constraint.
  /// \since 1.0
  None = 0,

  /// \brief Retain/Release.
  /// \since 1.0
  Retain_Release = 1,

  /// \brief Retain/Release for Simulator.
  /// \since 1.0
  Retain_Release_For_Simulator = 2,

  /// \brief Retain/Release or Garbage Collection.
  /// \since 1.0
  Retain_Release_Or_GC = 3,

  /// \brief Garbage Collection.
  /// \since 1.0
  GC = 4,
};

///
/// \brief Defines a list of supported file types.
/// \since 1.0
///
enum class FileType : unsigned {
  /// \brief Unsupported file type.
  /// \since 1.0
  Unsupported = 0,

  /// \brief Text-based Dynamic Library Stub File (.tbd) version 1.0
  /// \since 1.0
  TBD_V1 = 1,

  /// \brief Text-based stub file (.tbd) version 2.0
  /// \since 1.0
  TBD_V2 = 2,

  /// \brief Text-based stub file (.tbd) version 3.0
  /// \since 1.3
  TBD_V3 = 3,
};

///
/// \brief Defines the cpu subtype matching mode.
/// \since 1.0
///
enum class CpuSubTypeMatching : unsigned {
  /// \brief Fall-back to an ABI compatible slice if an exact match cannot be
  ///        found.
  /// \since 1.0
  ABI_Compatible = 0,

  /// \brief Only accept a slice if the sub type matches.
  /// \since 1.0
  Exact = 1,
};

///
/// \brief Defines flags that control the parsing of text-based stub files.
/// \since 1.1
///
enum ParsingFlags : unsigned {
  /// \brief Default flags.
  /// \since 1.1
  None = 0,

  /// \brief Only accept a slice if the sub type matches. ABI fall-back mode is
  ///        the default.
  /// \since 1.1
  ExactCpuSubType = 1U << 0,

  /// \brief Disallow weak imported symbols. This adds weak imported symbols to
  ///        the ignore exports list.
  /// \since 1.1
  DisallowWeakImports = 1U << 1,
};

inline ParsingFlags operator|(ParsingFlags lhs, ParsingFlags rhs) noexcept {
  return static_cast<ParsingFlags>(static_cast<unsigned>(lhs) |
                                   static_cast<unsigned>(rhs));
}

inline ParsingFlags operator|=(ParsingFlags &lhs, ParsingFlags rhs) noexcept {
  lhs = lhs | rhs;
  return lhs;
}

///
/// \brief TAPI File APIs
/// \since 1.0
///
class TAPI_PUBLIC LinkerInterfaceFile {
public:
  ///
  /// \brief Returns a list of supported file extensions.
  ///
  /// \returns a list of supported file extensions.
  /// \since 1.0
  ///
  static std::vector<std::string> getSupportedFileExtensions() noexcept;

  ///
  /// \brief Indicate if the provided buffer is a supported Text-based Dynamic
  ///        Library Stub file.
  ///
  /// Checks if the buffer is a supported format. This doesn't check for
  /// malformed buffer content.
  ///
  /// \param[in] path full path to the file.
  /// \param[in] data raw pointer to start of buffer.
  /// \param[in] size size of the buffer in bytes.
  /// \returns true if the format is supported.
  /// \since 1.0
  ///
  static bool isSupported(const std::string &path, const uint8_t *data,
                          size_t size) noexcept;

  ///
  /// \brief Check if we should prefer the text-based stub file.
  ///
  /// \param[in] path full path to the text-based stub file.
  /// \returns true if the tex-based stub file should be prefered over any
  ///          dynamic library.
  /// \since 1.0
  ///
  static bool shouldPreferTextBasedStubFile(const std::string &path) noexcept;

  ///
  /// \brief Check if the text-based stub file and the MachO dynamic library
  ///        file are in sync.
  ///
  /// This validates both files against each other and checks if both files are
  /// still in sync.
  ///
  /// \param[in] tbdPath full path to the text-based stub file.
  /// \param[in] dylibPath full path to the MachO dynamic library file.
  /// \returns true if both files are in sync.
  /// \since 1.0
  ///
  static bool areEquivalent(const std::string &tbdPath,
                            const std::string &dylibPath) noexcept;

  ///
  /// \brief Create a LinkerInterfaceFile from the provided buffer.
  ///
  /// Parses the content of the provided buffer with the given constrains for
  /// cpu type, cpu sub-type, matching requirement, and minimum deployment
  /// version.
  ///
  /// \param[in] path path to the file (for error message only).
  /// \param[in] data raw pointer to start of buffer.
  /// \param[in] size size of the buffer in bytes.
  /// \param[in] cpuType The cpu type / architecture to check the file for.
  /// \param[in] cpuSubType The cpu sub type / sub architecture to check the
  ///            file for.
  /// \param[in] matchingMode Specified the cpu subtype matching mode.
  /// \param[in] minOSVersion The minimum OS version / deployment target.
  /// \param[out] errorMessage holds an error message when the return value is a
  ///             nullptr.
  /// \return nullptr on error
  /// \since 1.0
  /// \deprecated 1.1
  ///
  static LinkerInterfaceFile *
  create(const std::string &path, const uint8_t *data, size_t size,
         cpu_type_t cpuType, cpu_subtype_t cpuSubType,
         CpuSubTypeMatching matchingMode, PackedVersion32 minOSVersion,
         std::string &errorMessage) noexcept __attribute__((deprecated));

  ///
  /// \brief Create a LinkerInterfaceFile from the provided buffer.
  ///
  /// Parses the content of the provided buffer with the given constrains for
  /// cpu type, cpu sub-type, flags, and minimum deployment version.
  ///
  /// \param[in] path path to the file (for error message only).
  /// \param[in] data raw pointer to start of buffer.
  /// \param[in] size size of the buffer in bytes.
  /// \param[in] cpuType The cpu type / architecture to check the file for.
  /// \param[in] cpuSubType The cpu sub type / sub architecture to check the
  ///            file for.
  /// \param[in] flags Flags that control the parsing behavior.
  /// \param[in] minOSVersion The minimum OS version / deployment target.
  /// \param[out] errorMessage holds an error message when the return value is a
  ///             nullptr.
  /// \return nullptr on error
  /// \since 1.1
  /// \deprecated 1.3
  ///
  static LinkerInterfaceFile *
  create(const std::string &path, const uint8_t *data, size_t size,
         cpu_type_t cpuType, cpu_subtype_t cpuSubType, ParsingFlags flags,
         PackedVersion32 minOSVersion, std::string &errorMessage) noexcept
      __attribute__((deprecated));

  ///
  /// \brief Create a LinkerInterfaceFile from a file.
  ///
  /// Parses the content of the file with the given constrains for cpu type,
  /// cpu sub-type, flags, and minimum deployment version.
  ///
  /// \param[in] path path to the file.
  /// \param[in] cpuType The cpu type / architecture to check the file for.
  /// \param[in] cpuSubType The cpu sub type / sub architecture to check the
  ///            file for.
  /// \param[in] flags Flags that control the parsing behavior.
  /// \param[in] minOSVersion The minimum OS version / deployment target.
  /// \param[out] errorMessage holds an error message when the return value is a
  ///             nullptr.
  /// \return nullptr on error
  /// \since 1.3
  ///
  static LinkerInterfaceFile *
  create(const std::string &path, cpu_type_t cpuType, cpu_subtype_t cpuSubType,
         ParsingFlags flags, PackedVersion32 minOSVersion,
         std::string &errorMessage) noexcept;

  ///
  /// \brief Query the file type.
  /// \return Returns the file type this TAPI file represents.
  /// \since 1.0
  ///
  FileType getFileType() const noexcept;

  ///
  /// \brief Query the platform
  /// \return Returns the platform supported by the TAPI file.
  /// \since 1.0
  /// \deprecated 1.6
  ///
  Platform getPlatform() const noexcept __attribute__((deprecated));

  ///
  /// \brief Query the supported platforms
  /// \return Returns the set of platforms supported by the TAPI file as
  ///         defined by the MachO load command LC_BUILD_VERSION.
  /// \since 1.6
  ///
  const std::vector<uint32_t> &getPlatformSet() const noexcept;

  ///
  /// \brief Query the install name.
  /// \return Returns the install name of the TAPI file.
  /// \since 1.0
  ///
  const std::string &getInstallName() const noexcept;

  ///
  /// \brief Query the install name is version specifc.
  /// \return True if the install name has been adjusted for the provided
  ///         minimum OS version.
  /// \since 1.0
  ///
  bool isInstallNameVersionSpecific() const noexcept;

  ///
  /// \brief Query the current library version.
  /// \return Returns the current library version as 32bit packed version.
  /// \since 1.0
  ///
  PackedVersion32 getCurrentVersion() const noexcept;

  ///
  /// \brief Query the compatibility library version.
  /// \return Returns the compatibility library version as 32bit packed version.
  /// \since 1.0
  ///
  PackedVersion32 getCompatibilityVersion() const noexcept;

  ///
  /// \brief Query the Swift ABI version.
  /// \return Returns the Swift ABI version as unsigned integer.
  /// \since 1.0
  ///
  unsigned getSwiftVersion() const noexcept;

  ///
  /// \brief Query the Objective-C Constraint.
  /// \return Returns always ObjCConstraint::None.
  /// \since 1.0
  /// \deprecated 1.6
  ///
  ObjCConstraint getObjCConstraint() const noexcept __attribute__((deprecated));

  ///
  /// \brief Query if the library has two level namespace.
  /// \return Returns true if the library has two level namespace.
  /// \since 1.0
  ///
  bool hasTwoLevelNamespace() const noexcept;

  ///
  /// \brief Query if the library is Applicatiuon Extension Safe.
  /// \return Returns true if the library is Application Extension Safe.
  /// \since 1.0
  ///
  bool isApplicationExtensionSafe() const noexcept;

  ///
  /// \brief Query if the library has any allowable clients.
  /// \return Return true if there are any allowable clients.
  /// \since 1.0
  ///
  bool hasAllowableClients() const noexcept;

  ///
  /// \brief Query if the library has any re-exported libraries.
  /// \return Return true if there are any re-exported libraries.
  /// \since 1.0
  ///
  bool hasReexportedLibraries() const noexcept;

  ///
  /// \brief Query if the library has any weak defined exports.
  /// \return Return true if there are any weak defined exports.
  /// \since 1.0
  ///
  bool hasWeakDefinedExports() const noexcept;

  ///
  /// \brief Obtain the name of the parent framework (umbrella framework).
  /// \return Returns the name of the parent framework (if it exists), otherwise
  ///         an empty string.
  /// \since 1.0
  ///
  const std::string &getParentFrameworkName() const noexcept;

  ///
  /// \brief Obtain the list of allowable clients.
  /// \return Returns a list of allowable clients.
  /// \since 1.0
  ///
  const std::vector<std::string> &allowableClients() const noexcept;

  ///
  /// \brief Obtain the list of re-exported libraries.
  /// \return Returns a list of re-exported libraries.
  /// \since 1.0
  ///
  const std::vector<std::string> &reexportedLibraries() const noexcept;

  ///
  /// \brief Obtain a list of all symbols to be ignored.
  /// \return Returns a list of all symbols that should be ignored.
  /// \since 1.0
  ///
  const std::vector<std::string> &ignoreExports() const noexcept;

  ///
  /// \brief Obtain a list of all exported symbols.
  /// \return Returns a list of all exported symbols.
  /// \since 1.0
  ///
  const std::vector<Symbol> &exports() const noexcept;

  ///
  /// \brief Obtain a list of all undefined symbols.
  /// \return Returns a list of all undefined symbols.
  /// \since 1.0
  ///
  const std::vector<Symbol> &undefineds() const noexcept;

  ///
  /// \brief Obtain a list of all inlined frameworks.
  /// \return Returns a list of install names of all inlined frameworks.
  /// \since 1.3
  ///
  const std::vector<std::string> &inlinedFrameworkNames() const noexcept;

  ///
  /// \brief Create a LinkerInterfaceFile from the specified inlined framework.
  ///
  /// Creates a LinkerInterfaceFile with the given constrains for cpu type,
  /// cpu sub-type, flags, and minimum deployment version.
  ///
  /// \param[in] installName install name of the inlined framework.
  /// \param[in] cpuType The cpu type / architecture to check the file for.
  /// \param[in] cpuSubType The cpu sub type / sub architecture to check the
  ///            file for.
  /// \param[in] flags Flags that control the parsing behavior.
  /// \param[in] minOSVersion The minimum OS version / deployment target.
  /// \param[out] errorMessage holds an error message when the return value is a
  ///             nullptr.
  /// \return nullptr on error
  /// \since 1.3
  ///
  LinkerInterfaceFile *
  getInlinedFramework(const std::string &installName, cpu_type_t cpuType,
                      cpu_subtype_t cpuSubType, ParsingFlags flags,
                      PackedVersion32 minOSVersion,
                      std::string &errorMessage) const noexcept;

  ///
  /// \brief Destructor.
  /// \since 1.0
  ///
  ~LinkerInterfaceFile() noexcept;

  ///
  /// \brief Copy constructor (deleted).
  /// \since 1.0
  ///
  LinkerInterfaceFile(const LinkerInterfaceFile &) noexcept = delete;
  LinkerInterfaceFile &operator=(const LinkerInterfaceFile &) noexcept = delete;

  ///
  /// \brief Move constructor.
  /// \since 1.0
  ///
  LinkerInterfaceFile(LinkerInterfaceFile &&) noexcept;
  LinkerInterfaceFile &operator=(LinkerInterfaceFile &&) noexcept;

private:
  LinkerInterfaceFile() noexcept;

  class Impl;
  std::unique_ptr<Impl> _pImpl;
};

TAPI_NAMESPACE_V1_END

#endif // TAPI_LINKER_INTERFACE_FILE_H
