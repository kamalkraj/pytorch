# Owner(s): ["oncall: quantization"]

import torch
import random
import unittest
from torch.ao.quantization.experimental.observer import APoTObserver
from torch.ao.quantization.experimental.quantizer import APoTQuantizer
from torch.ao.quantization.experimental.APoT_tensor import TensorAPoT

class TestQuantizedTensor(unittest.TestCase):
    r""" Tests int_repr on APoTQuantizer with random tensor2quantize
    and hard-coded values b=4, k=2
    """
    def test_int_repr(self):
        # generate random size of tensor2dequantize between 1 -> 20
        size = random.randint(1, 20)

        # generate tensor with random fp values between 0 -> 1000
        tensor2quantize = 1000 * torch.rand(size, dtype=torch.float)
        orig_tensor2quantize = torch.clone(tensor2quantize)

        observer = APoTObserver(b=4, k=2)

        observer.forward(tensor2quantize)

        qparams = observer.calculate_qparams(signed=False)

        quantizer = APoTQuantizer(alpha=observer.alpha, gamma=qparams[0], quantization_levels=qparams[1], level_indices=qparams[2])

        # get apot quantized tensor result
        qtensor = APoTQuantizer.quantize_APoT(tensor2quantize=tensor2quantize, quantization_levels=qparams[1], level_indices=qparams[2])

        tensor_apot = TensorAPoT(quantizer=quantizer, tensor2quantize=orig_tensor2quantize)

        qtensor_int_rep = tensor_apot.int_repr()

        self.assertTrue(torch.equal(qtensor, qtensor_int_rep))

if __name__ == '__main__':
    unittest.main()
