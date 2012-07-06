#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define IP (0)
#define SP (1)
#define RP (2)
#define A (3)
#define B (4)
#define END (5)

uint16_t code[1024]={};

int init_code(int datasize) {
	code[IP]=5+8*2+datasize;
	code[SP]=5-1;
	code[RP]=5+8-1;
	return code[IP];
}

static uint16_t tos() { return code[code[SP]]; }
static uint16_t pop() { return code[code[SP]--]; }
static void push(uint16_t v) { code[SP]++; code[code[SP]]=v; }

static uint16_t rpop() { return code[code[RP]--]; }
static void rpush(uint16_t v) { code[RP]++; code[code[RP]]=v; }

static uint16_t readop() { return code[code[IP]++]; }

void step() {
		uint8_t op=readop();
		switch(op) {
		// Reference; http://www.colorforth.com/inst.htm
		case 0x00: code[IP]=rpop(); break;
		case 0x01: { uint16_t r=rpop(); rpush(code[IP]); code[IP]=r; } break;
		case 0x02: code[IP]=readop(); break;
		case 0x03: rpush(code[IP]); code[IP]=readop(); break;
		case 0x04: // we don't have slots so no unext, sorry
		case 0x05: if(code[RP]) { code[IP]=readop(); } else { readop(); } break;
		case 0x06: if(!tos()) { code[IP]=readop(); } else { readop(); } break;
		case 0x07: if(tos()>0) { code[IP]=readop(); } else { readop(); } break;

		case 0x08: push(readop()); break;
		case 0x09: push(code[code[A]++]); break;
		case 0x0a: push(code[code[B]]); break;
		case 0x0b: push(code[code[A]]); break;
		case 0x0c: abort(); // port??? wtf is that
		case 0x0d: code[code[A]++]=pop(); break;
		case 0x0e: code[code[B]]=pop(); break;
		case 0x0f: code[code[A]]=pop(); break;

		case 0x10: push(pop()*pop()); break;
		case 0x11: push(pop()*2); break;
		case 0x12: push(pop()/2); break;
		case 0x13: push(-pop()); break;
		case 0x14: push(pop()+pop()); break;
		case 0x15: push(pop()&pop()); break;
		case 0x16: push(pop()^pop()); break;

		case 0x17: pop(); break;
		case 0x18: push(tos()); break;
		case 0x19: push(rpop()); break;
		case 0x1a: push(code[code[SP]-1]); break;
		case 0x1b: push(code[A]); break;
		case 0x1c: break;
		case 0x1d: rpush(pop()); break;
		case 0x1e: code[B]=pop(); break;
		case 0x1f: code[A]=pop(); break;

		case 0x98: exit(0); break;
		case 0x99: printf("0x%02x\n",tos()); break;

		default: printf("unknown op %02x\n",op);
		}
}

int main() {
	uint16_t i=init_code(16);
	code[i++]=0x08;
	code[i++]=0x123;
	code[i++]=0x08;
	code[i++]=0x1;
	code[i++]=0x14;
	code[i++]=0x99;
	code[i++]=0x98;
	

	for(;;) {
		step();
	}
	return 0;
}

