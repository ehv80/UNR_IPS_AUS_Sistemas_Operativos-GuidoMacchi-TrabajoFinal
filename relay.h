/*	relay.h
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _RELAY_H_RPCGEN
#define _RELAY_H_RPCGEN

#include <rpc/rpc.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef char *cadena;

struct manda_1_argument {
	cadena arg1;
	cadena arg2;
};
typedef struct manda_1_argument manda_1_argument;

#define RELAY 0x20000009
#define VRELAY 1
#define SIZE 1024                       /* <�� agregado */

#if defined(__STDC__) || defined(__cplusplus)
#define manda 1
extern  void * manda_1(cadena , cadena , CLIENT *);
extern  void * manda_1_svc(cadena , cadena , struct svc_req *);
#define retira 2
extern  cadena * retira_1(cadena , CLIENT *);
extern  cadena * retira_1_svc(cadena , struct svc_req *);
extern int relay_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define manda 1
extern  void * manda_1();
extern  void * manda_1_svc();
#define retira 2
extern  cadena * retira_1();
extern  cadena * retira_1_svc();
extern int relay_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_cadena (XDR *, cadena*);
extern  bool_t xdr_manda_1_argument (XDR *, manda_1_argument*);

#else /* K&R C */
extern bool_t xdr_cadena ();
extern bool_t xdr_manda_1_argument ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_RELAY_H_RPCGEN */
/*	relay.h */
