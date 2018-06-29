 DynamicArrayHelper dynamicarrayhelper;

void ProtocolBase::SetPulse(unsigned int *& pulsebuffer, byte & pulsebufferlength, byte pulsepos, unsigned int value)
{  
->dynamicarrayhelper.SetElementInArray( (void *&)pulsebuffer, &value , pulsepos , pulsebufferlength ,  sizeof(unsigned int) );
}