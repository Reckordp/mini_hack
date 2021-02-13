#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/array.h>

mrb_state *kendali = NULL;
FILE *source;
int kendali_serangan_dipanggil = 0;

void error_and_exit(char *);
void mrb_mruby_at_exit_gem_final(mrb_state*);

void kendali_serangan(int nomor) {
	if (kendali_serangan_dipanggil) raise(nomor);
	kendali_serangan_dipanggil = 1;
	if (kendali) mrb_mruby_at_exit_gem_final(kendali);
	signal(nomor, SIG_DFL);
	raise(nomor);
}

mrb_value load_dari_file(mrb_state *kendali, mrb_value self) {
	char *nama_file;
	mrb_get_args(kendali, "z", &nama_file);
	if (access(nama_file, R_OK) != 0) {
		mrb_raise(kendali, mrb_exc_get(kendali, "TypeError"), "file tidak tersedia!");
	}
	FILE *paket = fopen(nama_file, "r");
	mrb_load_file(kendali, paket);
	fclose(paket);
	return mrb_nil_value();
}

void error_and_exit(char *error) {
	char *e, *b, buffer[256];
	b = buffer;
	e = error;
	while(*e) *b++ = *e++;
	*b++ = 10;
	write(2, buffer, e - error + 1);
	exit(1);
}

void register_fungsi_ruby() {
	struct RClass *krn;
	krn = kendali->kernel_module;
	mrb_define_method(kendali, krn, "load", load_dari_file, MRB_ARGS_REQ(1));
}

int main(int argc, char** argv) {
	char buffer[256];
	mrb_value argv_ruby, param;

	signal(SIGTERM, kendali_serangan);
	signal(SIGINT, kendali_serangan);
	signal(SIGQUIT, kendali_serangan);
	signal(SIGKILL, kendali_serangan);
	signal(SIGHUP, kendali_serangan);

	kendali = mrb_open();
	argv_ruby = mrb_ary_new_capa(kendali, argc - 1);
	for(int urutan = 1; urutan < argc; ++urutan) {
		param = mrb_str_new_cstr(kendali, argv[urutan]);
		mrb_ary_set(kendali, argv_ruby, (mrb_int)(urutan - 1), param);
	}
	mrb_define_global_const(kendali, "ARGV", argv_ruby);

	register_fungsi_ruby();
	if (access("script.rb", R_OK) != 0) error_and_exit("script.rb tidak ditemukan.");
	source = fopen("script.rb", "r");
	mrb_load_file(kendali, source);
	fclose(source);
	if (kendali->exc) {
		mrb_print_backtrace(kendali);
		mrb_close(kendali);
		error_and_exit("Ruby program error");
	}
	mrb_close(kendali);
	return 0;
}
