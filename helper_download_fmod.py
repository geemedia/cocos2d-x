
import hashlib
import os
import platform
import subprocess
import sys
import shutil
from getpass import getpass
import xml.etree.ElementTree as ET
from threading import Thread

ON_POSIX = 'posix' in sys.builtin_module_names

# Prints the specified error message on stderr, on its own line, with a standard prefix, in red.
def print_error_message(message):
    """Prints an error message in red."""
    default_color = '\033[0;0m'
    red_color = '\033[01;31m'
    sys.stderr.write('{}ERROR: {}{}\n'.format(red_color, message, default_color))

def get_env_value_if_exists(env_var, default_value=''):
    """Gets the environment variable value if it exists, an empty string otherwise."""
    if env_var in os.environ:
        return os.environ[env_var]

    return default_value

def del_empty_dirs(the_dir):
    """Deletes all empty directories below the_dir."""
    empty = True
    for target in os.listdir(the_dir):
        path = os.path.join(the_dir, target)
        if os.path.isdir(path):
            if not del_empty_dirs(path):
                empty = False
        else:
            empty = False
    if empty:
        os.rmdir(the_dir)
    return empty

def directory_exists_or_die(the_dir):
    """Exits if the_dir does not exist."""
    if not os.path.exists(the_dir) or not os.path.isdir(the_dir):
        print_error_message('"{}" does not exist or is not a directory!'.format(the_dir))
        sys.exit(1)

def print_stdout(output, verbose, errors_in_stdout):
    """Prints output stream."""
    for line in iter(output.readline, b''):
        if verbose:
            sys.stdout.write(line)
        elif errors_in_stdout:
            sys.stderr.write(line)
    output.close()

def print_stderr(output):
    """Prints error stream."""
    for line in iter(output.readline, b''):
        sys.stderr.write(line)
    output.close()

def execute_command_with_success_or_die(cmd, error_message, verbose=False, errors_in_stdout=False): # pylint: disable=invalid-name
    """Executes command and exit if it fails."""
    outpipe = subprocess.PIPE
    if not verbose and not errors_in_stdout:
        outpipe = open(os.devnull, 'w')

    proc = subprocess.Popen(
        cmd, shell=True, bufsize=1, stdout=outpipe, stderr=subprocess.PIPE, close_fds=ON_POSIX)
    # we need to read from stdout and stderr to prevent them from getting full
    # (which causes deadlocks)

    if verbose or errors_in_stdout:
        out_thread = Thread(target=print_stdout, args=(proc.stdout, verbose, errors_in_stdout))
        out_thread.daemon = True # thread dies with the program
        out_thread.start()
    err_thread = Thread(target=print_stderr, args=(proc.stderr, ))
    err_thread.daemon = True # thread dies with the program
    err_thread.start()
    returncode = proc.wait()

    if returncode != 0:
        print_error_message(error_message)
        sys.exit(1)

def mount_orion_linux(prebuild_server, mount_point):
    """Mounts orion shared drive on Linux."""
    # Make sure the mount point exists, and is empty.
    if os.path.exists(mount_point):
        try:
            os.rmdir(mount_point)
        except OSError:
            print_error_message('{} is already mounted, no need to remove it.'.format(mount_point))
            return

    os.mkdir(mount_point)

    # Ask for user name and password.
    sys.stdout.write('Please enter your Windows username to mount (Orion) {}:\n'.format(prebuild_server))
    sys.stdout.flush()
    user_name = raw_input()
    user_password = getpass('Windows password for user {}:\n'.format(user_name))

    # Mount.
    cmd_mount = (
        'sudo mount -t cifs {} {} -o user={},pass={},file_mode=0777,dir_mode=0777').format(
            prebuild_server, mount_point, user_name, user_password)
    execute_command_with_success_or_die(
        cmd_mount, 'Failed to mount {} on {}'.format(prebuild_server, mount_point))

def unmount_orion_linux(mount_point):
    """Unmounts orion shared drive on Linux."""
    execute_command_with_success_or_die(
        'sudo umount {}'.format(mount_point), 'Failed to unmount {}'.format(mount_point))
    os.rmdir(mount_point)
