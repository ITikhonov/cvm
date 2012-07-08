#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define IP (0)
#define SP (1)
#define RP (2)
#define A (3)
#define B (4)
#define END (8)

static uint32_t tos(uint16_t *code) { uint32_t *c=(uint32_t*)code; return c[code[SP]]; }
static uint32_t pop(uint16_t *code) { uint32_t *c=(uint32_t*)code; return c[code[SP]--]; }
static void push(uint16_t *code, uint32_t v) { uint32_t *c=(uint32_t*)code; code[SP]++; c[code[SP]]=v; }
static void over(uint16_t *code) { uint32_t *c=(uint32_t*)code; push(code,c[code[SP]-1]); }

static uint16_t rpop(uint16_t *code) { return code[code[RP]--]; }
static void rpush(uint16_t *code,uint16_t v) { code[RP]++; code[code[RP]]=v; }

static uint16_t readop(uint16_t *code) { return code[code[IP]++]; }

void dump(uint16_t *c) {
	printf("0x%04x: op 0x%02x sp 0x%02x(0x%x) rp 0x%02x(0x%x)\n",c[IP],c[c[IP]],c[SP],tos(c),c[RP],c[c[RP]]);
}

uint16_t *step(uint16_t *c) {
		// dump(c);
		uint8_t op=readop(c);
		switch(op) {
		// Reference; http://www.colorforth.com/inst.htm
		case 0x00: c[IP]=rpop(c); break;
		case 0x01: { uint16_t r=rpop(c); rpush(c,c[IP]); c[IP]=r; } break;
		case 0x02: c[IP]=readop(c); break;
		case 0x03: { uint16_t r=readop(c); rpush(c,c[IP]); c[IP]=r; } break;
		case 0x04: // we don't have slots so no unext, sorry
		case 0x05: if(c[c[RP]]) { c[IP]=readop(c); c[c[RP]]--; } else { readop(c); rpop(c); } break;
		case 0x06: if(!tos(c)) { c[IP]=readop(c); } else { readop(c); } break;
		case 0x07: if(tos(c)>0) { c[IP]=readop(c); } else { readop(c); } break;

		case 0x08: push(c,readop(c)); break;
		case 0x09: push(c,c[c[A]++]); break;
		case 0x0a: push(c,c[c[B]]); break;
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
	uint16_t *code=malloc(65536);

	FILE *f=fopen(argv[1],"r");
	fread(code,1,65536,f);
	fclose(f);

	for(;;) {
		if(argc>2) dump(code);
		code=step(code);
	}
	return 0;
}

