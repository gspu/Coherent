/*
 * request.h
 * Nroff/Troff.
 * Request header.
 */

/* Request table structure. */
typedef struct {
	char q_name[2];
	int (*q_func)();
} REQ;

/*
 * Request functions.
 * The V7 "Nroff/Troff User's Manual" describes 83 requests.
 * Of these, 9 are currently unimplemented in COHERENT [nt]roff:
 *	.bd, .dt, .fc, .hc, .hw, .hy, .nm, .nn, .pi.
 * The 92 requests below also include 10 requests which are not in V7:
 *	.bo, .co, .dc, .fb, .fd, .lf, .nb, .rb, .rf, .rp.
 */
/* req1.c */
extern	int	req_ab();
extern	int	req_ad();
extern	int	req_af();
extern	int	req_am();
extern	int	req_as();
extern	int	req_bd();	/* unimplemented */
extern	int	req_bo();	/* !V7 */
extern	int	req_bp();
extern	int	req_br();
extern	int	req_c2();
extern	int	req_cc();
extern	int	req_ce();
extern	int	req_ch();
extern	int	req_co();	/* !V7 */
extern	int	req_cs();
extern	int	req_cu();
extern	int	req_da();
extern	int	req_dc();	/* !V7 */
extern	int	req_de();
extern	int	req_di();
extern	int	req_ds();
extern	int	req_dt();	/* unimplemented */
extern	int	req_ec();
extern	int	req_el();
extern	int	req_em();
extern	int	req_eo();
extern	int	req_ev();
extern	int	req_ex();
extern	int	req_fb();	/* !V7 */
extern	int	req_fc();	/* unimplemented */
extern	int	req_fd();	/* !V7 */
extern	int	req_fi();
extern	int	req_fl();
extern	int	req_fp();
extern	int	req_ft();
extern	int	req_fz();
extern	int	req_hc();	/* unimplemented */
extern	int	req_hw();	/* unimplemented */
extern	int	req_hy();	/* unimplemented */
extern	int	req_ie();
extern	int	req_if();
extern	int	req_ig();
extern	int	req_in();
extern	int	req_it();
extern	int	req_lc();
extern	int	req_lf();	/* !V7 */
extern	int	req_lg();
extern	int	req_ll();
extern	int	req_ls();
extern	int	req_lt();
extern	int	req_mc();
extern	int	req_mk();
/* req2.c */
extern	int	req_na();
extern	int	req_nb();	/* !V7 */
extern	int	req_ne();
extern	int	req_nf();
extern	int	req_nh();
extern	int	req_nm();	/* unimplemented */
extern	int	req_nn();	/* unimplemented */
extern	int	req_nr();
extern	int	req_ns();
extern	int	req_nx();
extern	int	req_os();
extern	int	req_pc();
extern	int	req_pi();	/* unimplemented */
extern	int	req_pl();
extern	int	req_pm();
extern	int	req_pn();
extern	int	req_po();
extern	int	req_ps();
extern	int	req_rb();	/* !V7 */
extern	int	req_rd();
extern	int	req_rf();	/* !V7 */
extern	int	req_rm();
extern	int	req_rn();
extern	int	req_rp();	/* !V7 */
extern	int	req_rr();
extern	int	req_rs();
extern	int	req_rt();
extern	int	req_so();
extern	int	req_sp();
extern	int	req_ss();
extern	int	req_sv();
extern	int	req_ta();
extern	int	req_tc();
extern	int	req_ti();
extern	int	req_tl();
extern	int	req_tm();
extern	int	req_tr();
extern	int	req_uf();
extern	int	req_ul();
extern	int	req_vs();
extern	int	req_wh();

/* end of request.h */
