from django.http import HttpResponse


def index(request):
    return HttpResponse("Point your minecraft server here")
