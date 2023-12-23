//
// Created by 10315 on 2023/12/18.
//

#include "communicator.h"

void Communicator::sendSignal(SIGNALS signal) {
    emit this->messageSent(signal);
}