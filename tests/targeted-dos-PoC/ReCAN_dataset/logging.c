int main() {
  while (1) {
    if (CAN0.isBusOff()) {
      GPIO_write(LED0, true);
      GPIO_write(DBG_0, true);
    }
    if (CAN1.isBusOff()) {
      GPIO_write(LED1, true);
      GPIO_write(DBG_1, true);
    }
  }
}