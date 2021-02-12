# !/usr/bin/env ruby
# frozen_string_literal: false

require 'nokogiri'

TEMPAT_INFO = IO.pipe.each { |pipe| pipe.sync = true }
PERINTAH_INFO = proc do |perintah|
	pid = spawn(perintah, out: TEMPAT_INFO.last)
	Process.wait pid
	TEMPAT_INFO.first.read_nonblock(2**(2**4)).chomp
end

MRUBY_LIB_PATH = %q(lib).freeze
MRUBY_INCLUDE_PATH = %q(include).freeze

GCC_MARKAS = %q(/usr/lib/x86_64-linux-gnu).freeze
GCC_PRINT_NAME = proc { PERINTAH_INFO.call("gcc --print-file-name=#{Regexp.last_match(1)}") }
GCC_FLAG = %q(-nostdlib -nostartfiles -static).freeze
GCC_LIBRARY = %q(-L %s -lmruby -lm).freeze
GCC_CRT_A = %q(@crt1.o @crti.o @crtbegin.o).gsub(/@([\w\d]+\.\w)/, &GCC_PRINT_NAME).freeze
GCC_CRT_S = %q(@crtend.o @crtn.o).gsub(/@([\w\d]+\.\w)/, &GCC_PRINT_NAME).freeze
GCC_SYSTEM = %q(@libc.a -lgcc -lgcc_eh).gsub(/@([\w\d]+\.\w)/, &GCC_PRINT_NAME).freeze
PERINTAH_GCC_OBJECT = %q(gcc -g -c %s -I %s -o %s).freeze
PERINTAH_GCC_LINKER_A = "#{GCC_FLAG} -o %s #{GCC_CRT_A} %s #{GCC_CRT_S} #{GCC_LIBRARY}".freeze
PERINTAH_GCC_LINKER_B = "-Wl,--start-group #{GCC_SYSTEM} -Wl,--end-group".freeze
PERINTAH_GCC_LINKER = %q(gcc ).concat(PERINTAH_GCC_LINKER_A, ' ', PERINTAH_GCC_LINKER_B).freeze

REMOVE_COMMAND = %q(rm -rf %s).freeze
RUN_COMMAND = %q(./%s).freeze

SRC_PROGRAM = %q(miniMruby.c).freeze
OBJECT_SINGGAH = %q(mini.o).freeze
PROGRAM_NAME = %q(mini).freeze

file 'lib/libmruby.a' do 
	sh 'curl -k' + 
	' https://github.com/Reckordp/mruby/releases/download/V11/libmruby.a' + 
	' -o laman_mruby'
	laman = Nokogiri::HTML.parse(File.read('laman_mruby'))
	link = laman.css('a').attribute('href').value
	sh 'curl -k ' + link + ' -o lib/libmruby.a'
end

task default: 'lib/libmruby.a' do
	sh 'bundle install'
	mruby_environment = [MRUBY_INCLUDE_PATH, MRUBY_LIB_PATH]
	sh PERINTAH_GCC_OBJECT % [SRC_PROGRAM, mruby_environment[0], OBJECT_SINGGAH]
	sh PERINTAH_GCC_LINKER % [PROGRAM_NAME, OBJECT_SINGGAH, mruby_environment[1]]
	sh REMOVE_COMMAND % [OBJECT_SINGGAH]
end

