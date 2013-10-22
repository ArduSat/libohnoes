#!/usr/bin/env python
# encoding: utf-8

import os
import time

APPNAME = 'libohnoes'
VERSION = '1.0'

def options(ctx):
  ctx.load('gcc gas')

  # Global options
  ctx.add_option('--toolchain', action='store', default='', help='Set toolchain prefix')

  # Group options
  gr = ctx.add_option_group('libohnoes options')
  gr.add_option('--with-freertos', metavar='PATH', default='../libgomspace/include', help='Set path to FreeRTOS header files')
  gr.add_option('--includes', default='', help='Add additional include paths. Separate with comma')

def configure(ctx):
  ctx.env.CC = ctx.options.toolchain + "gcc"
  ctx.env.AR = ctx.options.toolchain + "ar"
  ctx.load('gcc')
  ctx.find_program(ctx.options.toolchain + 'size', var='SIZE')

  ctx.env.append_unique('INCLUDES', [ 'include' ] + ctx.options.includes.split(','))
  ctx.env.append_unique('INCLUDES', [ ctx.options.with_freertos ])

  ctx.env.append_unique('CFLAGS', [
          '-Wall', '-Wextra', '-Wno-unused-parameter', '-fshort-enums',
          '-Wno-missing-field-initializers', '-Os', '-std=gnu99',
          '-funsigned-char', '-funsigned-bitfields', '-fdata-sections',
          '-ffunction-sections', '-Wno-cast-align' ])

def build(ctx):
  install_path = False

  ctx.stlib(source=ctx.path.ant_glob('src/**/*.c'),
            target='libohnoes',
            includes = ctx.env.INCLUDES,
            export_includes = [ 'include' ],
            cflags = ctx.env.CFLAGS,
            use = 'include',
            install_path = install_path)
