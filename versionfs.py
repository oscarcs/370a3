#!/usr/bin/env python
from __future__ import with_statement

import logging

import os
import sys
import errno
import shutil
import re

from fuse import FUSE, FuseOSError, Operations, LoggingMixIn


class VersionFS(LoggingMixIn, Operations):
    def __init__(self):
        # get current working directory as place for versions tree
        self.root = os.path.join(os.getcwd(), '.versiondir')

        # create dict
        self.modified = {}

        # check to see if the versions directory already exists
        if os.path.exists(self.root):
            print 'Version directory already exists.'
        else:
            print 'Creating version directory.'
            os.mkdir(self.root)

    # Helpers
    # =======

    def _full_path(self, partial):
        if partial.startswith("/"):
            partial = partial[1:]
        path = os.path.join(self.root, partial)
        return path

    # Filesystem methods
    # ==================

    def access(self, path, mode):
        '''
        Test for access to a particular path
        '''
        # print "access:", path, mode
        full_path = self._full_path(path)
        if not os.access(full_path, mode):
            raise FuseOSError(errno.EACCES)

    def chmod(self, path, mode):
        '''
        Change the mode of a particular path
        '''
        # print "chmod:", path, mode
        full_path = self._full_path(path)
        return os.chmod(full_path, mode)

    def chown(self, path, uid, gid):
        '''
        Change the owner of a particular path
        '''
        # print "chown:", path, uid, gid
        full_path = self._full_path(path)
        return os.chown(full_path, uid, gid)

    def getattr(self, path, fh=None):
        '''
        Return file attributes by calling lstat() 
        '''
        # print "getattr:", path
        full_path = self._full_path(path)
        st = os.lstat(full_path)
        return dict((key, getattr(st, key)) for key in ('st_atime', 'st_ctime',
                     'st_gid', 'st_mode', 'st_mtime', 'st_nlink', 'st_size', 'st_uid'))

    def readdir(self, path, fh):
        '''
        Read a directory, get all of the contents.
        '''
        # print "readdir:", path
        full_path = self._full_path(path)

        dirents = ['.', '..']
        if os.path.isdir(full_path):
            
            items = os.listdir(full_path)
            new_items = []
            for i in items:
                last_extension = i[i.rindex('.')+1:]
                if not re.match("[0-9]+", last_extension):
                    new_items.append(i)

            dirents.extend(new_items)

        for r in dirents:

            # Filter out the versions of each file: 
            # print 
            yield r

    def readlink(self, path):
        # print "readlink:", path
        pathname = os.readlink(self._full_path(path))
        if pathname.startswith("/"):
            # Path name is absolute, sanitize it.
            return os.path.relpath(pathname, self.root)
        else:
            return pathname

    def mknod(self, path, mode, dev):
        # print "mknod:", path, mode, dev
        return os.mknod(self._full_path(path), mode, dev)

    def rmdir(self, path):
        # print "rmdir:", path
        full_path = self._full_path(path)
        return os.rmdir(full_path)

    def mkdir(self, path, mode):
        # print "mkdir:", path, mode
        return os.mkdir(self._full_path(path), mode)

    def statfs(self, path):
        # print "statfs:", path
        full_path = self._full_path(path)
        stv = os.statvfs(full_path)
        return dict((key, getattr(stv, key)) for key in ('f_bavail', 'f_bfree',
            'f_blocks', 'f_bsize', 'f_favail', 'f_ffree', 'f_files', 'f_flag',
            'f_frsize', 'f_namemax'))

    def unlink(self, path):
        # print "unlink:", path
        return os.unlink(self._full_path(path))

    def symlink(self, name, target):
        # print "symlink:", name, target
        return os.symlink(target, self._full_path(name))

    def rename(self, old, new):
        '''
        Rename files and directories.
        '''
        # print "rename:", old, new

        # rename all of the versions:
        for v in range(1, 7):
            try:
                os.rename(self._full_path(old + '.' + str(v)), self._full_path(new + '.' + str(v)))
            except os.error:
                pass

        return os.rename(self._full_path(old), self._full_path(new))

    def link(self, target, name):
        # print "link:", target, name
        return os.link(self._full_path(name), self._full_path(target))

    def utimens(self, path, times=None):
        # print "utimens:", path, times
        return os.utime(self._full_path(path), times)

    # File methods
    # ============

    def open(self, path, flags):
        print '** open:', path, '**'
        full_path = self._full_path(path)
        return os.open(full_path, flags)

    def create(self, path, mode, fi=None):
        print '** create:', path, '**'
        full_path = self._full_path(path)
        return os.open(full_path, os.O_WRONLY | os.O_CREAT, mode)

    def read(self, path, length, offset, fh):
        print '** read:', path, '**'
        os.lseek(fh, offset, os.SEEK_SET)
        return os.read(fh, length)

    def write(self, path, buf, offset, fh):
        """
        Write to a file.
        """
        print '** write:', path, '**'
        os.lseek(fh, offset, os.SEEK_SET)

        # Mark the file as modified.
        # We have to use a dictionary in order to track multiple open files.
        try:
            if path[path.rindex('/')+1] != '.':        
                self.modified[path] = True
        except Exception as e: print(e)

        return os.write(fh, buf)

    def truncate(self, path, length, fh=None):
        """
        Truncate a file.
        """
        print '** truncate:', path, '**'
        full_path = self._full_path(path)

        # Mark the file as modified
        # We have to use a dictionary in order to track multiple open files.
        try:
            if path[path.rindex('/')+1] != '.':
                self.modified[path] = True
        except Exception as e: print(e)

        with open(full_path, 'r+') as f:
            f.truncate(length)

    def flush(self, path, fh):
        """
        Flush a file (save changes to disk).
        """
        print '** flush', path, '**'

        if path in self.modified and self.modified[path]:
            # Remove the last version
            if os.path.exists(self._full_path(path + '.6')):
                os.remove(self._full_path(path + '.6'))

            # Move down the old versions
            for v in reversed(range(1, 7)):
                try:
                    os.rename(self._full_path(path + '.' + str(v)), self._full_path(path + '.' + str(v + 1)))
                except:
                    pass

            try:
                # Create a new version .1
                shutil.copyfile(self._full_path(path), self._full_path(path + '.1'))
            except:
                print "Unable to create latest file version for file", path

            # Prevent memory leak
            del self.modified[path]

        return os.fsync(fh)

    def release(self, path, fh):
        print '** release', path, '**'
        return os.close(fh)

    def fsync(self, path, fdatasync, fh):
        print '** fsync:', path, '**'
        return self.flush(path, fh)


def main(mountpoint):
    FUSE(VersionFS(), mountpoint, nothreads=True, foreground=True)

if __name__ == '__main__':
    # logging.basicConfig(level=logging.DEBUG)
    main(sys.argv[1])