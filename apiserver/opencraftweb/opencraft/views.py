from django.http import HttpResponse
import json

def index(request):
    return HttpResponse("Point your minecraft server here")

def authenticate(request):
    return HttpResponse("")
