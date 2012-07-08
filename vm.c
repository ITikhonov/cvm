#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define IP (0)
#define SP (1)
#define RP (2)
#define A (3)
#define B (4)
#define END (5)

static uint16_t tos(uint16_t *code) { return code[code[SP]]; }
static uint16_t pop(uint16_t *code) { return code[code[SP]--]; }
static void push(uint16_t *code, uint16_t v) { code[SP]++; code[code[SP]]=v; }

static uint16_t rpop(uint16_t *code) { return code[code[RP]--]; }
static void rpush(uint16_t *code,uint16_t v) { code[RP]++; code[code[RP]]=v; }

static uint16_t readop(uint16_t *code) { return code[code[IP]++]; }

void dump(uint16_t *c) {
	printf("0x%04x: op 0x%02x sp 0x%02x\n",c[IP],c[c[IP]],c[SP]);
}

void step(uint16_t *c) {
		// dump(c);
		uint8_t op=readop(c);
		switch(op) {
		// Reference; http://www.colorforth.com/inst.htm
		case 0x00: c[IP]=rpop(c); break;
		case 0x01: { uint16_t r=rpop(c); rpush(c,c[IP]); c[IP]=r; } break;
		case 0x02: c[IP]=readop(c); break;
		case 0x03: { uint16_t r=readop(c); rpush(c,c[IP]); c[IP]=r; } break;
		case 0x04: // we don't have slots so no unext, sorry
		case 0x05: if(c[RP]) { c[IP]=readop(c); } else { readop(c); } break;
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
		case 0x1a: push(c,c[c[SP]-1]); break;
		case 0x1b: push(c,c[A]); break;
		case 0x1c: break;
		case 0x1d: rpush(c,pop(c)); break;
		case 0x1e: c[B]=pop(c); break;
		case 0x1f: c[A]=pop(c); break;

		case 0x98: exit(0); break;
		case 0x99: printf("0x%02x\n",tos(c)); break;

		default:
			printf("unknown op 0x%02x at 0x%x\n",op,c[IP]);
			abort();
		}
}

int main(int argc,char *argv[]) {
	uint16_t code[1024];

	FILE *f=fopen(argv[1],"r");
	fread(code,1,sizeof(code),f);
	fclose(f);

	for(;;) {
		step(code);
	}
	return 0;
}

