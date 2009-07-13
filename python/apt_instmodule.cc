// -*- mode: cpp; mode: fold -*-
// Description								/*{{{*/
// $Id: apt_instmodule.cc,v 1.3 2002/01/08 06:53:04 jgg Exp $
/* ######################################################################

   apt_intmodule - Top level for the python module. Create the internal
                   structures for the module in the interpriter.

   Note, this module shares state (particularly global config) with the
   apt_pkg module.

   ##################################################################### */
									/*}}}*/
// Include Files							/*{{{*/
#include "apt_instmodule.h"
#include "generic.h"

#include <apt-pkg/debfile.h>
#include <apt-pkg/error.h>

#include <sys/stat.h>
#include <unistd.h>
#include <Python.h>
									/*}}}*/

// debExtractControl - Exctract an arbitary control member		/*{{{*/
// ---------------------------------------------------------------------
/* This is a common operation so this function will stay, but others that
   expose the full range of the apt-inst .deb processing will join it some
   day. */
static char *doc_debExtractControl =
"deb_extract_control(file[,member]) -> String\n"
"Returns the indicated file from the control tar. The default is 'control'\n";
static PyObject *debExtractControl(PyObject *Self,PyObject *Args)
{
   char *Member = "control";
   PyObject *File;
   if (PyArg_ParseTuple(Args,"O|s",&File,&Member) == 0)
      return 0;

   // Subscope makes sure any clean up errors are properly handled.
   PyObject *Res = 0;
   {
      // Open the file and associate the .deb
      int fileno = PyObject_AsFileDescriptor(File);
      if (fileno == -1)
         return 0;
      FileFd Fd(fileno,false);
      debDebFile Deb(Fd);
      if (_error->PendingError() == true)
	 return HandleErrors();

      debDebFile::MemControlExtract Extract(Member);
      if (Extract.Read(Deb) == false)
	 return HandleErrors();

      // Build the return result

      if (Extract.Control == 0)
      {
	 Py_INCREF(Py_None);
	 Res = Py_None;
      }
      else
	 Res = PyString_FromStringAndSize(Extract.Control,Extract.Length+2);
   }

   return HandleErrors(Res);
}
									/*}}}*/

// debExtractArchive - Exctract the archive		/*{{{*/
// ---------------------------------------------------------------------
static char *doc_debExtractArchive =
"deb_extract_archive(File,rootdir) -> Bool\n"
"Extracts the Archive into the given root dir";
static PyObject *debExtractArchive(PyObject *Self,PyObject *Args)
{
   char *Rootdir = NULL;
   char cwd[512];
   PyObject *File;
   if (PyArg_ParseTuple(Args,"O|s",&File,&Rootdir) == 0)
      return 0;

   // Subscope makes sure any clean up errors are properly handled.
   bool res = false;
   {
      if(Rootdir != NULL)
      {
	 getcwd(cwd, sizeof(cwd));
	 chdir(Rootdir);
      }

      // Open the file and associate the .deb
      int fileno = PyObject_AsFileDescriptor(File);
      if (fileno == -1)
         return 0;
      FileFd Fd(fileno,false);
      debDebFile Deb(Fd);
      if (_error->PendingError() == true) {
	 if (Rootdir != NULL)
	    chdir (cwd);
	 return HandleErrors(Py_BuildValue("b",false));
      }

      // extracts relative to the current dir
      pkgDirStream Extract;
      res = Deb.ExtractArchive(Extract);

      if (Rootdir != NULL)
	 chdir (cwd);
      if (res == false)
	 return HandleErrors(Py_BuildValue("b",res));
   }
   return HandleErrors(Py_BuildValue("b",res));
}
									/*}}}*/
// arFindMember - Find member in AR archive              		/*{{{*/
// ---------------------------------------------------------------------
static char *doc_arCheckMember =
"ar_check_member(file, membername) -> Bool\n";
static PyObject *arCheckMember(PyObject *Self,PyObject *Args)
{
   char *Member = NULL;
   bool res = false;
   PyObject *File;
   if (PyArg_ParseTuple(Args,"Os",&File,&Member) == 0)
      return 0;

   // Open the file and associate the .deb
   int fileno = PyObject_AsFileDescriptor(File);
   if (fileno == -1)
      return 0;
   FileFd Fd(fileno,false);
   ARArchive AR(Fd);
   if (_error->PendingError() == true)
      return HandleErrors(Py_BuildValue("b",res));

   if(AR.FindMember(Member) != 0)
      res = true;

   return HandleErrors(Py_BuildValue("b",res));
}
									/*}}}*/

// initapt_inst - Core Module Initialization				/*{{{*/
// ---------------------------------------------------------------------
/* */
static PyMethodDef methods[] =
{
   // access to ar files
   {"ar_check_member", arCheckMember, METH_VARARGS, doc_arCheckMember},

   // access to deb files
   {"deb_extract_control",debExtractControl,METH_VARARGS,doc_debExtractControl},
   {"deb_extract_archive",debExtractArchive,METH_VARARGS,doc_debExtractArchive},

   // access to tar streams
   {"tar_extract",tarExtract,METH_VARARGS,doc_tarExtract},
   {"deb_extract",debExtract,METH_VARARGS,doc_debExtract},

#ifdef COMPAT_0_7
   {"arCheckMember", arCheckMember, METH_VARARGS, doc_arCheckMember},
   {"debExtractControl",debExtractControl,METH_VARARGS,doc_debExtractControl},
   {"debExtractArchive",debExtractArchive,METH_VARARGS,doc_debExtractArchive},
   {"tarExtract",tarExtract,METH_VARARGS,doc_tarExtract},
   {"debExtract",debExtract,METH_VARARGS,doc_debExtract},
#endif
   {}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "apt_inst",
        "Functions for working with packages and ar,tar archives (apt-inst)",
        -1,
        methods,
        0,
        0,
        0,
        0
};

extern "C" PyObject * PyInit_apt_inst()
{
   return PyModule_Create(&moduledef);
}
#else
extern "C" void initapt_inst()
{
   Py_InitModule("apt_inst",methods);
}
#endif									/*}}}*/
