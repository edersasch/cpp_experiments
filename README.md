# C++ Experiments

Concepts, solved problems and examples, available under [MIT license](LICENSE)

* Archive Helpers
  * [arhelper_qt](#arhelper_qt): Uses 7z binary to handle archive content
  * [ar_history_search_qt](#ar_history_search_qt): Store recent archives and filter the content
* Elementary Math
  * [elementary_math_qml](#elementary_math_qml): Visualizations for math operations in elementary school
  * [emvis1000_qml](#emvis1000_qml): Highlight solutions to plus / minus / multiply questions in the number range up to 1000
* File System Helpers
  * [fs_filter_qt](#fs_filter_qt): Searchable tree view of a directory
  * [fs_history_qt](#fs_history_qt): Maintains a string list of existing files or directories in the order of last access
  * [fs_history_search_qt](#fs_history_search_qt): Store nine recent directories and filter the content


# Build Requirements

* C++ 17 toolchain (e.g. [gcc](https://gcc.gnu.org/) >= 7.3.0)
* [CMake](https://cmake.org) >= 3.10.0
* [Qt](https://qt.io) >= 5.11.0


# Optional Tools

* [GTest](https://github.com/google/googletest) >= 1.10.0, downloaded automatically if option `'EXPERIMENTS_BUILD_TESTS` is `ON`
* [Doxygen](http://www.doxygen.nl) documentation tool, enables `apidoc` build target
* GCov (part of [gcc](https://gcc.gnu.org/)) / [LCov](http://ltp.sourceforge.net/coverage/lcov.php) for coverage report if option `EXPERIMENTS_COVERAGE_REPORT` is `ON`


# Directory Structure

The top level structure follows [the pitchfork layout](https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs).


## `docs/`

[Changelog.md](docs/Changelog.md)


## `src/`


### arhelper_qt

Runtime dependencies:

* `7z` executable in `PATH`

Experimenting with handling archive content.

Enable build with cmake option `EXPERIMENTS_ARHELPER_QT`.


### ar_history_search_qt

Application that combines `arhelper_qt` and `fs_history_qt` to provide filtering
to archive contents like [fs_history_search_qt](#fs_history_search_qt).

Enabled if `EXPERIMENTS_ARHELPER_QT` and `EXPERIMENTS_FS_HISTORY_QT` is on.


### elementary_math_qml

The directory contains a QML file and its C++ bindings to highlight solutions
to plus / minus / multiply questions in the number range up to 1000

Enable build with cmake option `EXPERIMENTS_ELEMENTARY_MATH_QML`.


### emvis1000_qml

The application uses functions from `elementary_math_qml` to highlight
solutions to plus / minus / multiply questions in the number range up to 1000.

Enabled if `EXPERIMENTS_ELEMENTARY_MATH_QML` is on.


### fs_filter_qt

This widget shows a line edit above a tree view of an existing directory.
The line edit filters all visible rows in the tree in a case insensitive way.
You can expand every subdirectory automatically, so everything in the
directory will be visible. This may take some time, so use only for not so
huge directories, depending on your machine and patience.
Directories that become empty during filtering and do not contain the filter
text can optionally be hidden. If the the filter line edit is empty, all
directories are shown, even those that are empty without filtering.

Double clicking an element opens it in the associated application.
Multiple elements can be selected, dragging is supported.

Enable build with cmake option `EXPERIMENTS_FS_FILTER_QT`.


### fs_history_qt

The string list can either be used with files or with directories.
Every element is watched whether it goes out of existence, which triggers
removal from the list.
The default list size is `9`, so a hotkey can be applied to a menu entry.
If the list is used on directories, you can provide a fallback directory
which will be chosen if all other entries become invalid.

Some UI elements are provided that utilize the history list:
* combobox and optional combobox label style to show text elided left
* menu that adds a number as hotkey to each entry if the list size is `9` or less
* toolbutton to start a select file or directory dialog

Please try to only use the `current_element_changed(const QString&)` or
`elements_changed(const QStringList&)` signals, because the widgets might fire
twice when the list gets sorted because of a widget's trigger.

Enable build with cmake option `EXPERIMENTS_FS_HISTORY_QT`.


### fs_history_search_qt

Application that combines `fs_filter_qt` and `fs_history_qt`. You have an
initially empty combobox. To the right of it is a button to start a directory
selection. Once a valid directory is selected, its content is shown below in a
tree view that expands automatically, so please use only on not so huge
directories. Between the combobox and the tree view is a line edit where you
can filter the visible entries of the tree. If a directory gets emptied by
filtering, it is hidden from view if its name does not contain the filter
string.

Enabled if `EXPERIMENTS_FS_FILTER_QT` and `EXPERIMENTS_FS_HISTORY_QT` are on.


## `tests/`

Unit tests are
[placed separately](https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs#src.tests.separate)
from their sources, because measuring test coverage becomes incorrect if the
test resides in the same directory as the source files. Test code coverage is
of course 100%, so the overall coverage would look better than it is. Naming
unit test files is done as described in the pitchfork paragraph
[Merged Test Placement](https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs#src.tests.merged).

The directories whose names match a directory name from `src/` contain unit
tests.


# Included Resources

CMake file `AddGoogleTest.cmake` adapted from
<https://github.com/CLIUtils/cmake/blob/4e52e4d0bc2e9fd27171926d0b5d9f396dd8637c/AddGoogleTest.cmake>
is licensed under a
[BSD 3-Clause License](https://github.com/CLIUtils/cmake/blob/master/LICENSE) ([local copy](docs/LICENSE_CLIUtils_cmake)):
use URL instead of GIT

CMake file `CodeCoverage.cmake` adapted from
<https://github.com/bilke/cmake-modules/blob/7727aa773b8d72afe6801436af9e60cb1688351e/CodeCoverage.cmake>
is licensed under a BSD 3-Clause License included in the file itself: commented
out `COMMAND ${LCOV_PATH} --gcov-tool ${GCOV_PATH} --remove ${Coverage_NAME}.total ${COVERAGE_LCOV_EXCLUDES} ...`
and added `COMMAND ${LCOV_PATH} --gcov-tool ${GCOV_PATH} --extract ${Coverage_NAME}.total ${COVERAGE_LCOV_INCLUDES} ...`

CMake files
[DownloadProject.CMakeLists.cmake.in](https://github.com/Crascit/DownloadProject/blob/8adf96570b583fd1c280bed8f43a9a0528670cf4/DownloadProject.CMakeLists.cmake.in)
and [DownloadProject.cmake](https://github.com/Crascit/DownloadProject/blob/26983dd7883acbabf6fd98968429f9af44186baf/DownloadProject.cmake)
are licensed under
[MIT License](https://github.com/Crascit/DownloadProject/blob/master/LICENSE) ([local copy](docs/LICENSE_Crascit_DownloadProject)).
