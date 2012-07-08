#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define IP (0)
#define SP (4)
#define RP (8)
#define A (12)
#define B (16)

static uint32_t *p32(uint8_t *c,uint32_t i) { return (uint32_t*)&c[i]; };
static uint32_t ip(uint8_t *c) { return *p32(c,IP); }
static uint32_t sp(uint8_t *c) { return *p32(c,SP); }
static uint32_t rp(uint8_t *c) { return *p32(c,RP); }
static uint32_t a(uint8_t *c) { return *p32(c,A); }
static uint32_t b(uint8_t *c) { return *p32(c,B); }

static void spinc(uint8_t *c,int n) { *p32(c,SP)+=n; }
static void ipinc(uint8_t *c,int n) { *p32(c,IP)+=n; }
static void rpinc(uint8_t *c,int n) { *p32(c,RP)+=n; }

static uint32_t tos(uint8_t *c) { return *p32(c,sp(c)); }
static uint32_t pop(uint8_t *c) { uint32_t x=tos(c); spinc(c,-4); return x; }
static void push(uint8_t *c,uint32_t v) { spinc(c,4); *p32(c,sp(c))=v; }
static void over(uint8_t *c) { push(c,*p32(c,sp(c)-4)); }

static uint32_t *ptor(uint8_t *c) { return p32(c,rp(c)); }
static uint32_t rpop(uint8_t *c) { uint32_t x=*ptor(c); rpinc(c,-4); return x; }
static void rpush(uint8_t *c,uint32_t v) { rpinc(c,4); *p32(c,rp(c))=v; }


static uint8_t readop(uint8_t *c) { uint8_t x=c[ip(c)]; ipinc(c,1); return x; }

void dump(uint8_t *c) {
	printf("0x%04x: op 0x%02x sp 0x%02x(0x%x) rp 0x%02x(0x%x)\n",ip(c),c[ip(c)],sp(c),tos(c),rp(c),*ptor(c));
}

uint8_t *step(uint8_t *c) {
		uint32_t *ip=p32(c,IP);
		uint32_t *sp=p32(c,SP);
		uint32_t *rp=p32(c,RP);
		uint32_t *a=p32(c,A);
		uint32_t *b=p32(c,B);

		// dump(c);
		uint8_t op=readop(c);
		switch(op) {
		// Reference; http://www.colorforth.com/inst.htm
		case 0x00: *ip=rpop(c); break;
		case 0x01: { uint8_t r=rpop(c); rpush(c,c[IP]); c[IP]=r; } break;
		case 0x02: c[IP]=readop(c); break;
		case 0x03: { uint8_t r=readop(c); rpush(c,c[IP]); c[IP]=r; } break;
		case 0x04: // we don't have slots so no unext, sorry
		case 0x05: if(*ptor(c)) { c[IP]=readop(c); (*ptor(c))--; } else { readop(c); rpop(c); } break;
		case 0x06: if(!tos(c)) { c[IP]=readop(c); } else { readop(c); } break;
		case 0x07: if(tos(c)>0) { c[IP]=readop(c); } else { readop(c); } break;

		case 0x08: push(c,readop(c)); break;
		case 0x09: push(c,c32[c[A]]); break;
		case 0x0a: push(c,c32[c[B]]); break;
		case 0x0b: push(c,c[c[A]]); break;
		case 0x0c: abort(); // port??? wtf is that
		case 0x0d: c[c[A]++]=pop(c); break;
		case 0x0e: c[c[B]]=pop(c); break;
		case 0x0f: c[c[A]]=pop(c); break;

		case 0x10: push(c,pop(c)*pop(c)); break;
		case 0x11: push(c,pop(c)*2); break;
		case 0x12: push(c,pop(c)/2); break;
		case 0x13: push(c,-pop(c)); break;
		case 0x14: push(c,pop(c)+pop(c)); break;
		case 0x15: push(c,pop(c)&pop(c)); break;
		case 0x16: push(c,pop(c)^pop(c)); break;

		case 0x17: pop(c); break;
		case 0x18: push(c,tos(c)); break;
		case 0x19: push(c,rpop(c)); break;
		case 0x1a: over(c); break;
		case 0x1b: push(c,c[A]); break;
		case 0x1c: break;
		case 0x1d: rpush(c,pop(c)); break;
		case 0x1e: c[B]=pop(c); break;
		case 0x1f: c[A]=pop(c); break;

		case 0x96: c=realloc(c,pop(c)<<16); break;
		case 0x97: printf("0x%02x ",tos(c)); break;
		case 0x98: printf("%c\n",tos(c)); break;
		case 0x99: exit(0); break;

		default:
			printf("unknown op 0x%02x at 0x%x\n",op,c[IP]);
			abort();
		}
	return c;
}

int main(int argc,char *argv[]) {
	uint8_t *code=malloc(65536);

	FILE *f=fopen(argv[1],"r");
	fread(code,1,65536,f);
	fclose(f);

	for(;;) {
		if(argc>2) dump(code);
		code=step(code);
	}
	return 0;
}

