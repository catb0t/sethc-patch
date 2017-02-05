#include <dirent.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>

/*
  if path to windows installation is provided then only use that
  if a path to scan is provided, test all the subfolders for windows installations
  if a path selected as a windows installation is determined to have the patch applied,
*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
#pragma GCC diagnostic ignored "-Wmissing-prototypes"
// help the user
bool wh_help ();
bool wh_help () {
  puts("usage: winhak [--help] (d[o] | u[ndo]) [path [paths...]]");
  return EXIT_FAILURE;
}
#pragma GCC diagnostic pop

// move a file
bool   wh_fmove (const char* const path);
// remove a file
bool wh_fremove (const char* const path);

// perform the setch patch on a Windows installation directory
bool   wh_do_patch (const char* const win_path);
// revert the sethc patch
bool wh_undo_patch (const char* const win_path);

// perform the setch patch on each Windows installation in the list
bool   wh_do_patch_each (const char* const * const paths);
// revert the patch on each given Windows installation directory
bool wh_undo_patch_each (const char* const * const paths);

// is path a Windows installation root directory?
bool wh_is_win_root (const char* const path);

// attempt to automagically patch any Windows installations in /media and /mnt
bool wh_try_autopatch (void);

// attempt to automagically unpatch Windows roots in /media, /mnt
bool wh_try_autounpatch (void);

// name of Command Prompt
#define WH_WIN_CMD_EXE_FNAME "cmd.exe"
#define WH_WIN_CMD_EXE_FQPN  "/Windows/System32/cmd.exe"

// name of Sticky Keys helper
#define WH_WIN_SETHC_EXE_FNAME "sethc.exe"
#define WH_WIN_SETHC_EXE_FQPN  "/Windows/System32/sethc.exe"

// name of the backup location of the Sticky Keys helper
#define WH_WIN_SETHC_BAK_FNAME WIN_SETHC_EXE_FNAME ## ".old"
#define WH_WIN_SETHC_BAK_FQPN  WIN_SETHC_EXE_FQPN  ## ".old"

/*static*/ const char auto_search_dirs[][40] = {
  "/media/",
  "/mnt/",
  "/home/%s/mount/" // getlogin_r
};


// usage: winhak [--help] (d[o] | u[ndo]) [path [paths...]]

int main(int argl, const char* const * const args) {

  switch (argl - 1) {

    // $0
    case 0: {
      wh_help();
      return EXIT_FAILURE;
    }

    // $0 (d[o] | u[ndo])
    case 1: {
      char lc_arg = (char) tolower(args[1][0]);
      bool res = (
        (lc_arg == 'd')
        ? wh_try_autopatch
        : (lc_arg == 'u')
          ? wh_try_autounpatch
          : wh_help
      ) ();
      return res;
    }

    // $0 (d[o] | u[ndo]) /path/
    case 2: {
      char lc_arg = (char) tolower(args[1][0]);
      bool res = (
        (lc_arg == 'd')
        ? wh_do_patch
        : (lc_arg == 'u')
          ? wh_undo_patch
          : wh_help
      ) (args[2]);
      return res;
    }

    // $0 (d[o] | u[ndo]) /path/ /path2/ ...
    default: {
      char lc_arg = (char) tolower(args[1][0]);
      bool res = (
        (lc_arg == 'd')
        ? wh_do_patch_each
        : (lc_arg == 'u')
          ? wh_undo_patch_each
          : wh_help
      ) (args + 2);
      return res;
    }

  }

  // to do:
  // rename Windows/System32/sethc.exe -> sethc.exe.old
  // copy Windows/System32/cmd.exe -> sethc.exe

  // to undo:
  // delete Windows/System32/sethc.exe
  // rename Windows/System32/sethc.exe.old -> sethc.exe
  return EXIT_SUCCESS;

}

