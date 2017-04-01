import uuid
from datetime import datetime
from django.db import models


class User(models.Model):
   UserID               = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
   UserUsername         = models.CharField(max_length=30)
   UserEmail            = models.EmailField()
   UserDOB              = models.DateField(default=datetime.now)
   UserEmailVerified    = models.BooleanField(default=False)
   UserEmailVerifyToken = models.UUIDField(default=uuid.uuid4, editable=False)
   UserRegistered       = models.DateTimeField(auto_now_add=True)
   UserPasswordChange   = models.DateTimeField(default=datetime.now)
   UserPasswordHash     = models.CharField(max_length=60,default="")
