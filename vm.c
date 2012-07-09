#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void *debug=0;

static uint32_t *p32(uint8_t *c,uint32_t i) { return (uint32_t*)&c[i]; };

uint8_t *step(uint8_t *c) {
		uint32_t *ip=p32(c,0);
		uint32_t *sp=p32(c,4);
		uint32_t *rp=p32(c,8);
		uint32_t *a=p32(c,12);
		uint32_t *b=p32(c,16);

		uint32_t *p(uint32_t i) { return (uint32_t*)(&c[i]); };

		uint32_t tos() { return *p(*sp); }
		uint32_t pop() { uint32_t x=tos(); *sp-=4; return x; }
		void push(uint32_t v) { *p(*sp+=4)=v; }
		void over() { push(*p(*sp-4)); }

		uint32_t *ptor() { return p(*rp); }
		uint32_t rpop() { uint32_t x=*ptor(); *rp-=4; return x; }
		void rpush(uint32_t v) { *p(*rp+=4)=v; }

		uint8_t readop() { return c[(*ip)++]; }
		uint32_t readint() { uint32_t x=*p(*ip); *ip+=4; return x; }


		void dump() {
			uint32_t *idx=debug;
			int m=*idx++;
			char *w=debug;
			char *s="[outofrange]";
			if(*ip<m) {
				int i=idx[*ip];
				s=i?(w+i):"[unknown]";
			}
			fprintf(stderr,"0x%04x ",*ip);
			fprintf(stderr,"%-20s:\top 0x%02x sp 0x%02x(0x%x) rp 0x%02x(0x%x)\n",
					s,c[*ip],*sp,tos(),*rp,*ptor());
		}

		////////////////////////////////////////////////////

		if(debug) dump();

		uint8_t op=readop();
		switch(op) {
		// Reference; http://www.colorforth.com/inst.htm
		case 0x00: *ip=rpop(); break;
		case 0x01: { uint32_t r=rpop(); rpush(*ip); *ip=r; } break;
		case 0x02: *ip=readint(); break;
		case 0x03: { uint32_t r=readint(); rpush(*ip); *ip=r; } break;
		case 0x04: // we don't have slots so no unext, sorry
		case 0x05: if(*ptor()) { *ip=readint(); (*ptor())--; } else { readint(); rpop(); } break;
		case 0x06: if(!tos()) { *ip=readint(); } else { readint(); } break;
		case 0x07: if(tos()>0) { *ip=readint(); } else { readint(); } break;

		case 0x08: push(readint()); break;
		case 0x09: push(*p(*a)); *a+=4; break;
		case 0x0a: push(*p(*b)); break;
		case 0x0b: push(*p(*a)); break;
		case 0x0c: abort(); // port??? wtf is that
		case 0x0d: *p(*a)=pop(c); *a+=4; break;
		case 0x0e: *p(*b)=pop(c); break;
		case 0x0f: *p(*a)=pop(c); break;

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
		case 0x1a: over(); break;
		case 0x1b: push(*a); break;
		case 0x1c: break;
		case 0x1d: rpush(pop()); break;
		case 0x1e: *b=pop(); break;
		case 0x1f: *a=pop(); break;

		case 0x30: c[*a]=pop(); break; // c!
		case 0x31: push(c[*a]); break; // c@

		case 0x96: c=realloc(c,pop()<<16); break;
		case 0x97: printf("0x%02x ",tos()); break;
		case 0x98: printf("%c\n",tos()); break;
		case 0x99: exit(0); break;

		default:
			printf("unknown op 0x%02x at 0x%x\n",op,*ip);
			abort();
		}
	return c;
}

int main(int argc,char *argv[]) {
	uint8_t *code=malloc(65536);
	
	FILE *f=fopen(argv[1],"r");
	fread(code,1,65536,f);
	fclose(f);

	if(argc>2) {
		debug=malloc(65536);
		FILE *f=fopen(argv[2],"r");
		fread(debug,1,65536,f);
		fclose(f);
	}

	for(;;) {
		code=step(code);
	}
	return 0;
}

