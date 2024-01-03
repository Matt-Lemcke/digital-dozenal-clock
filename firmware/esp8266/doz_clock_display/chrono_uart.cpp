#include "chrono_uart.h"

#define UART_BUFFER_SIZE  100

static uint8_t uart_buffer[UART_BUFFER_SIZE];
static uint8_t buffer_index = 0;

typedef enum
{
  Idle,
  Ready,
  ReadBitmap,
  ReadColour,
  ReadBrightness
}StateCode;

typedef struct
{
  StateCode state_code;

  void (*in)(void);
  void (*process)(void);
  void (*out)(void);
}State;

typedef struct
{
  Chrono  *ctx;
  State   *curr_state;
}StateMachine;

static void Default_In(void);
static void Default_Process(void);
static void Default_Out(void);
static void Idle_In(void);
static void Idle_Process(void);
static void Ready_Process(void);
static void Read_Process(void);
static void ReadBitmap_Out(void);
static void ReadColour_Out(void);
static void ReadBrightness_Out(void);

static void transition(State *next_state);
static void clearBuffer(void);

static StateMachine g_chrono;

State s_idle = 
{
  .state_code = Idle,
  .in = Idle_In,
  .process = Idle_Process,
  .out = Default_Out
};

State s_ready = 
{
  .state_code = Ready,
  .in = Default_In,
  .process = Ready_Process,
  .out = Default_Out
};

State s_read_bitmap =
{
  .state_code = ReadBitmap,
  .in = Default_In,
  .process = Read_Process,
  .out = ReadBitmap_Out
};

State s_read_colour =
{
  .state_code = ReadColour,
  .in = Default_In,
  .process = Read_Process,
  .out = ReadColour_Out
};

State s_read_brightness =
{
  .state_code = ReadBrightness,
  .in = Default_In,
  .process = Read_Process,
  .out = ReadBrightness_Out
};

void Chrono_Init(Chrono *ctx, uint32_t br)
{
  Serial.begin(br);
  g_chrono.ctx = ctx;
  g_chrono.curr_state = &s_idle;
  g_chrono.curr_state->in();
}

void Chrono_Update()
{
  if(!(Serial.available()>0)){return;}
  
  if(Serial.peek() == '\n'){          // Remove later
    Serial.read();
    return;
  }  
  g_chrono.curr_state->process();
}

void Idle_In(void)
{
  clearBuffer();
}

void Idle_Process(void)
{
  uint8_t c = Serial.read();
  if(c == START_CODE)
  {
  transition(&s_ready);
  }
}

void Ready_Process(void)
{
  uint8_t c = Serial.read();
  switch(c)
  {
  case BITMAP_CODE:
    transition(&s_read_bitmap);
    break;
  case COLOUR_CODE:
    transition(&s_read_colour);
    break;
  case BRIGHTNESS_CODE:
    transition(&s_read_brightness);
    break;
  case OFF_CODE:
    g_chrono.ctx->display_off();
    transition(&s_idle);
    break;
  case ON_CODE:
    g_chrono.ctx->display_on();
    transition(&s_idle);
    break;
  case START_CODE:
  case END_CODE:
    transition(&s_idle);
    break;
  default:
    break;
  }
}

void Read_Process(void)
{
  uint8_t c = Serial.read();
  if(c == END_CODE || c == START_CODE)
  {
    transition(&s_idle);
  }
  else
  {
    uart_buffer[buffer_index] = c;
    buffer_index++;
    if(buffer_index >= UART_BUFFER_SIZE)
    {
      transition(&s_idle);
    }
  }
}

void ReadBitmap_Out(void)
{
  g_chrono.ctx->setBitmap(uart_buffer[0], uart_buffer+1);
}
void ReadColour_Out(void)
{
  g_chrono.ctx->setColour(uart_buffer[0], uart_buffer[1]);
}
void ReadBrightness_Out(void)
{
  g_chrono.ctx->setMatrixBrightness(uart_buffer[0]);
}

void Default_In(void)
{
}
void Default_Process(void)
{
}
void Default_Out(void)
{
}

void transition(State *next_state)
{
  g_chrono.curr_state->out();
  g_chrono.curr_state = next_state;
  g_chrono.curr_state->in();
  Serial.printf("\nState entry: %d", g_chrono.curr_state->state_code);
}

void clearBuffer(void)
{
  memset(uart_buffer, 0, UART_BUFFER_SIZE);
  buffer_index = 0;
}
