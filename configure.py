# vim: set sts=2 ts=8 sw=2 tw=99 et:
import sys
from ambuild2 import run

# Simple extensions do not need to modify this file.

builder = run.PrepareBuild(sourcePath = sys.path[0])

builder.options.add_option('--hl2sdk-root', type=str, dest='hl2sdk_root', default=None,
		                   help='Root search folder for HL2SDKs')
builder.options.add_option('--mms-path', type=str, dest='mms_path', default=None,
                       help='Path to Metamod:Source')
builder.options.add_option('--sm-path', type=str, dest='sm_path', default=None,
                       help='Path to SourceMod')
builder.options.add_option('--enable-debug', action='store_const', const='1', dest='debug',
                       help='Enable debugging symbols')
builder.options.add_option('--enable-optimize', action='store_const', const='1', dest='opt',
                       help='Enable optimization')
builder.options.add_option('--enable-lto', action='store_const', const='1', dest='lto',
                       help='Enable link time optimization')
builder.options.add_option('--build-id', type=str, dest='build_id', default=None,
                       help='Build ID')

builder.Configure()
