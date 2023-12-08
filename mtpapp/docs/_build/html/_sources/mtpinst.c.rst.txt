.. c:function:: void MTP_sock_init (struct MTP_sock *mtpsk, struct MTP *mtpinst)

   Constructor for MTP_sock objects. This function initializes only the parts of the socket that are owned by MTP.

.. container:: kernelindent

  **Parameters**

  ``struct MTP_sock *mtpsk``
    Object to initialize.

  ``struct MTP *mtpinst``
    MTP implementation that will manage the socket.

  **Return**

  always 0 (success).


.. c:function:: struct MTP * MTP_init (void)

   Constructor for MTP object. This function initializes MTP data structure.

.. container:: kernelindent

  **Parameters**

  ``void``
    no arguments

  **Return**

  pointer to the MTP object.


