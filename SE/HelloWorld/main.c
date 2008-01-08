#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}

u16 timer; // ID �������
int lamp=0; // ������� ������� �����
int n=10; // ������� ������ �������

void onTimer (u16 * unk , void * data)
{
  // ���� ������� �� ����� �� ����
  if(n--)
  {
    // ������������� ������
    Timer_ReSet(&timer,500,onTimer,0);
    // ����������� �����
    SetLampLevel(lamp^=0x10);
  }
  else
  {
    // ������� "�������" �� StandBy
    StatusIndication_ShowNotes(0x6FFFFFFF);
    // ��������� �����
    SetLampLevel(0);
    // ��������� ������ � ��������� ����� �� ������
    SUBPROC(elf_exit);
  }
}

int main (void)
{
  // ������� ����� � "�������" �� StandBy
  StatusIndication_ShowNotes(STR("Hello World!"));
  // ��������� ������ �� 0.5 ���.
  timer=Timer_Set(500,onTimer,0);
  return(0);
}
