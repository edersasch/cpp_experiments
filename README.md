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
* Note Taking
  * [Tiddlerstore](#tiddlerstore): text storage based on [tiddlywiki](https://tiddlywiki.com) [tiddler](https://tiddlywiki.com#Tiddlers) concept
  * [Tiddlerstore_qt](#tiddlerstore_qt): Qt tools that utilize [tiddlerstore](#tiddlerstore)
  * [Tiddler_Inspector_qt](#tiddler_inspector_qt): text based editor for tiddlers
* [QTableView Demo](#qtableview_demo): Code related to QTableView
* [Qt Utilities](#qt_utilities)
  * `Combobox_Label_Elide_Left_Proxy_Style`: If the text is too long for the label it will be elided left
  * `tableview_adjust_row_visibility()`: hide rows that became empty
  * `tableview_resizeColumnsToContents_maxpercent()`: resize column width up to a maximum if necessary
  * `treeview_hide_expand()`: hides rows in a tree view that don't match a given pattern


# Build Requirements

* C++ 17 toolchain (e.g. [gcc](https://gcc.gnu.org/) >= 7.3.0)
* [CMake](https://cmake.org) >= 3.11.0
* [Qt](https://qt.io) >= 5.12.0
* [nlohmann/json](https://github.com/nlohmann/json/) >= 3.4.0 for [tiddlerstore](#tiddlerstore), downloaded automatically if option `EXPERIMENTS_USE_EXTERNAL_JSON` is `OFF` (license: [MIT](https://github.com/nlohmann/json/blob/v3.4.0/LICENSE.MIT))


# Optional Tools

* [GTest](https://github.com/google/googletest) >= 1.10.0, downloaded automatically if option `EXPERIMENTS_BUILD_TESTS` is `ON`
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

This library can create a model of the archive contents and extract a file or a
tree to a given directory.

Enable build with cmake option `EXPERIMENTS_ARHELPER_QT`.


### ar_history_search_qt

Application that combines `arhelper_qt` and `fs_history_qt` to provide
filtering to archive contents like
[fs_history_search_qt](#fs_history_search_qt). Double clicking a file or
directory extracts it to a temporary folder and opens it in the operating
system's default application.

Enabled if `EXPERIMENTS_ARHELPER_QT`, `EXPERIMENTS_FS_HISTORY_QT` and
`EXPERIMENTS_FS_FILTER_QT` is on.


### elementary_math_qml

The directory contains a QML file and its C++ bindings to highlight solutions
to plus / minus / multiply questions in the number range up to 1000

Enable build with cmake option `EXPERIMENTS_ELEMENTARY_MATH_QML`.


### emvis1000_qml

The application uses functions from `elementary_math_qml` to highlight
solutions to plus / minus / multiply questions in the number range up to 1000.

Enabled if `EXPERIMENTS_ELEMENTARY_MATH_QML` is on.


### fs_filter_qt

This widget shows a line edit above a tree view of an existing directory. The
line edit filters all visible rows in the tree in a case insensitive way. You
can expand every subdirectory automatically, so everything in the directory
will be visible. This may take some time, so use only for not so huge
directories, depending on your machine and patience. Directories that become
empty during filtering and do not contain the filter text can optionally be
hidden. If the filter line edit is empty, all directories are shown, even
those that are empty without filtering.

Double clicking an element opens it in the associated application. Multiple
elements can be selected, dragging is supported.

Enable build with cmake option `EXPERIMENTS_FS_FILTER_QT`.


### fs_history_qt

The string list can either be used with files or with directories.
Every element is watched whether it goes out of existence, which triggers
removal from the list.
The default list size is `9`, so a hotkey can be applied to a menu entry.
If the list is used on directories, you can provide a fallback directory
which will be chosen if all other entries become invalid.

Some UI elements are provided that utilize the history list:
* combobox
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


### qtableview_demo

Application that demonstrates several features of a QTableView.

* horizontal scrolling for long cell: difference between "by item" and "by pixel"
* initial column sizes up to a maximum: resize column width up to a maximum if necessary
* hide rows in sparse tables: if a column gets hidden and only empty cells are left in a row, hide the row

Enable build with cmake option `EXPERIMENTS_QTABLEVIEW_DEMO`.


### Qt_Utilities

`Combobox_Label_Elide_Left_Proxy_Style`: If the text is too long for the label
it will be elided left

`FlowLayout`: BSD licensed flowlayout class from Qt examples, license is
contained in the source files itself.

`Qt_Utilities::tableview_adjust_row_visibility()`: decide row visibility depending on
decoration, background, disabled cells

`Qt_Utilities::tableview_resizeColumnsToContents_maxpercent()`: resize column width up to a
maximum if necessary

`Qt_Utilities::treeview_hide_expand()`: hides rows in a tree view that don't match a given
pattern and optionally fully expands the view

Enable build with cmake option `EXPERIMENTS_ARHELPER_QT` or `EXPERIMENTS_FS_FILTER_QT`
or `EXPERIMENTS_QTABLEVIEW_DEMO` or `EXPERIMENTS_TIDDLER_INSPECTOR_QT`.


### tiddlerstore

If you are looking for a great note taking application that also has the
ability to store files and that runs everywhere please give [tiddlywiki](https://tiddlywiki.com)
a try. Its concepts are used here to store text information

* title
* text with an adjustable content history
* tags (like a named bool: either present or not)
* fields (key / value pairs)
* lists (key / value list pairs)

together in one unit. Conversion to and from json is done using
[nlohmann/json](https://github.com/nlohmann/json/).

Enable build with cmake option `EXPERIMENTS_TIDDLER_INSPECTOR_QT`


### tiddlerstore_qt

`Tiddler_Model`: operates on a Tiddler and emits signals if a part of the Tiddler changes

Enable build with cmake option `EXPERIMENTS_TIDDLER_INSPECTOR_QT`


### tiddler_inspector_qt

Text based editor for tiddlers

Enable build with cmake option `EXPERIMENTS_TIDDLER_INSPECTOR_QT`


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

Icon taken from [openclipart](https://openclipart.com) is licensed under
[Creative Commons Zero 1.0 Public Domain License](https://openclipart.org/share):

* emvis1000.svg ([two.svg](https://openclipart.org/detail/71101/two-animal))
