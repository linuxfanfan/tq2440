/* -*- Mode: C++ -*-
 * Worldvisions Tunnel Vision Software:
 *   Copyright (C) 1997-2002 Net Integration Technologies, Inc.
 *
 * RSA cryptography abstractions.
 */
#ifndef __WVRSA_H
#define __WVRSA_H

#include "wverror.h"
#include "wvencoder.h"
#include "wvencoderstream.h"

struct rsa_st;

/**
 * An RSA public key or public/private key pair that can be used for
 * encryption.
 * 
 * Knows how to encode/decode itself into a string of hex digits
 * for easy transport.
 * 
 * @see WvRSAEncoder
 */
class WvRSAKey : public WvErrorBase
{
    WvString pub, prv;

    void init(WvStringParm keystr, bool priv);
    static WvString hexifypub(struct rsa_st *rsa);
    static WvString hexifyprv(struct rsa_st *rsa);

public:
    struct rsa_st *rsa;

    WvRSAKey(const WvRSAKey &k);
    WvRSAKey(struct rsa_st *_rsa, bool priv); // note: takes ownership

    /**
     * Populate the RSA key with a hexified() key 
     */
    WvRSAKey(WvStringParm keystr, bool priv);

    /**
     * Create a new RSA key of bits strength.
     */
    WvRSAKey(int bits);
    
    ~WvRSAKey();
    
    virtual bool isok() const;
    
    /**
     * Retrieve the private key as a hexified string
     * returns WvString::null if there is only a public
     * key.
     */
    WvString private_str() const
        { return prv; }

    /**
     * Retrieve the public key as a hexified string
     */
    WvString public_str() const
        { return pub; }
    
    /**
     * Retrieve the public or private key in PEM encoded
     * format.
     */
    WvString getpem(bool privkey);

};


/**
 * An encoder implementing the RSA public key encryption method.
 * 
 * This encoder really slow, particularly for decryption, so should
 * only be used to negotiate session initiation information.  For
 * more intensive work, consider exchanging a key for use with a
 * faster symmetric cipher like Blowfish.
 * 
 * Supports reset().
 * 
 */
class WvRSAEncoder : public WvEncoder
{
public:
    enum Mode {
        Encrypt,     /*!< Encrypt with public key */
        Decrypt,     /*!< Decrypt with private key */
        SignEncrypt, /*!< Encrypt digital signature with private key */
        SignDecrypt  /*!< Decrypt digital signature with public key */
    };

    /**
     * Creates a new RSA cipher encoder.
     * 
     * "mode" is the encryption mode
     * "key" is the public key if mode is Encrypt or SignDecrypt,
     *            otherwise the private key
     */
    WvRSAEncoder(Mode mode, const WvRSAKey &key);
    virtual ~WvRSAEncoder();

protected:
    virtual bool _encode(WvBuf &in, WvBuf &out, bool flush);
    virtual bool _reset(); // supported

private:
    Mode mode;
    WvRSAKey key;
    size_t rsasize;
};


/**
 * A crypto stream implementing RSA public key encryption.
 * 
 * By default, written data is encrypted using WvRSAEncoder::Encrypt,
 * read data is decrypted using WvRSAEncoder::Decrypt.
 * 
 * @see WvRSAEncoder
 */
class WvRSAStream : public WvEncoderStream
{
public:
    WvRSAStream(WvStream *_cloned,
        const WvRSAKey &_my_key, const WvRSAKey &_their_key, 
        WvRSAEncoder::Mode readmode = WvRSAEncoder::Decrypt,
        WvRSAEncoder::Mode writemode = WvRSAEncoder::Encrypt);
    virtual ~WvRSAStream() { }
};


#endif // __WVRSA_H
