import tink
from tink import hybrid

def encrypt_json_object(json_object):
    print("Encrypting JSON Object")

    message = json.dumps(json_object)
    context = b'context'

    # Register all Hybrid primitives
    hybrid.register()

    # 1. Generate the private key material.
    private_keyset_handle = tink.new_keyset_handle(hybrid.hybrid_key_templates.ECIES_P256_HKDF_HMAC_SHA256_AES128_GCM)

    # Obtain the public key material.
    public_keyset_handle = private_keyset_handle.public_keyset_handle()

    # Encryption

    # 2. Get the primitive.
    hybrid_encrypt = public_keyset_handle.primitive(hybrid.HybridEncrypt)

    # 3. Use the primitive.
    ciphertext = hybrid_encrypt.encrypt(message.encode(), context)

    return ciphertext
